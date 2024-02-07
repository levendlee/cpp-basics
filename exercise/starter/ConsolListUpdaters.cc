#include "ConsolList.h"
#include "ConsolListUpdaters.h"

// These functions will be invoked roughly in the order in which their
// definitions are listed. Consider this example:
//
// ConsolListEntryUpdater::insert(a)
// ...
// ConsolListEntryUpdater::insert(b)
//
// Now a purchase is made:
//
// ConsolListPurchaseUpdater::onPurchaseBegin()
// ConsolListPurchaseUpdater::onMatch(a)
// ConsolListPurchaseUpdater::onMatch(..)
// ConsolListPurchaseUpdater::onReentry(..)
// ConsolListPurchaseUpdater::onPurchaseEnd()
//
// The ConsolListPurchaseUpdater object is constructed prior to the first onMatch(),
// and will be destroyed after onPurchaseEnd() is called.

void ConsolListEntryUpdater::insert(ConsolList &cl, const Offer &_newOffer)
{
    // This function is invoked once when a farmer makes a new offer.
    PublicOffer publicOffer(_newOffer);
    auto iter = lower_bound(cl.begin(), cl.end(), publicOffer);
    // TODO: This runs super slow.
    // insert before.
    cl.insert(iter, publicOffer);
}

void ConsolListPurchaseUpdater::onPurchaseBegin()
{
    // This function is called when a purchase starts.
    // From here on, until onPurchaseEnd() is called, the ConsolList is not observed by
    // anything, so you can keep it in any state you wish.
}

void ConsolListPurchaseUpdater::onMatch(const Offer &offer)
{
    // onMatch is invoked everytime a purchase is (partly) successful.
    // The member variables displayedQty/qty of offer are already updated.

    // 1) Checks buffered offers.
    auto bufferedOffersIter = bufferedOffers_.find(offer.oid.refNum);
    if (bufferedOffersIter != bufferedOffers_.end())
    {
        PublicOffer &publicOffer = bufferedOffersIter->second;
        assert(publicOffer.price == offer.price);
        assert(publicOffer.oid == offer.oid);
        publicOffer.displayedQty = offer.displayedQty;

        if (!publicOffer.displayedQty)
        {
            bufferedOffers_.erase(bufferedOffersIter);
        }
        return;
    }

    // 2) Check soriginal offers.
    PublicOffer publicOffer(offer);
    // `price` and `oid` must match. `displayedQty` might not.
    publicOffer.displayedQty = 0;
    auto ConsolListIter = lower_bound(cl->begin(), cl->end(), publicOffer);
    assert(ConsolListIter->price == offer.price);
    assert(ConsolListIter->oid == offer.oid);
    ConsolListIter->displayedQty = offer.displayedQty;
    // Keep it open as zeros to be cleaned up later.
}

void ConsolListPurchaseUpdater::onReentry(const Offer &icebergOffer)
{
    // onReentry is invoked when the icebergOffer has just been restocked
    // and reinserted in the offer list.
    // The member variables displayedQty/qty and oid are already updated.

    // 1) Buffers the icebergeOffer and leaves the consolidatedList unchanged.
    PublicOffer publicOffer(icebergOffer);
    bufferedOffers_[icebergOffer.oid.refNum] = publicOffer;
}

void ConsolListPurchaseUpdater::onPurchaseEnd()
{
    // This function is called when a purchase, that bought at least one apple, is finished.
    // The ConsolListPurchaseUpdater is destroyed after this call.
    
    // 1) Sorts and the buffered offers and stores them.
    set<PublicOffer> bufferedOffersTree;
    for (auto& [refNum, offer] : bufferedOffers_) {
        bufferedOffersTree.insert(move(offer));
    }

    // 2) Reversely construct the list from the end.
    // Since the updater can only *reduce* the size of consolidated list
    // instead of *increase* the size, so the final consolidated list is
    // guarantee to be able to hold all the final offers.
    // Use two pointers and construct from the end to the from so that we
    // can also read pointer will always be at the same location or higher
    // before the write pointer, since the iceberg offer can only move offers
    // upwards, and the eliminated offers can only read offers upwards as well.

    // Reverse iterator to iterate from the best offers to the worst.
    auto bufferedOffersTreeRIter = bufferedOffersTree.rbegin();

    const int n = cl->size();
    int readIndex = n - 1, writeIndex = n - 1;
    while (readIndex >= 0 ||
           bufferedOffersTreeRIter != bufferedOffersTree.rend()) {
        if ((readIndex < 0) ||
            (bufferedOffersTreeRIter != bufferedOffersTree.rend() &&
             cl->at(readIndex) < *bufferedOffersTreeRIter)) {
            // The offer in the buffer is a better offer;
            // TODO: Figure out why cannot use move here.
            (*cl)[writeIndex] = *bufferedOffersTreeRIter;
            --writeIndex;
            ++bufferedOffersTreeRIter;
        }
    }
}
