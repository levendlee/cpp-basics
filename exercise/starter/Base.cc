// ************************
// IMPORTANT. You will not need to read this. All the information you need should be in the document,
// and if you have questions, don't hesitate to ask your interviewer.
// ************************

#include "Base.h"
#include "ConsolList.h"
#include "ConsolListUpdaters.h"

unsigned MatchingEngine::nextRefNum = 1;

void MatchingEngine::addOffer(Offer o, bool addToCL) {
    o.oid = {farmerID.c_str(), nextRefNum++};
    offers.insert(o);
    if (addToCL)
        ConsolListEntryUpdater::insert(cl, o);
}

void MatchingEngine::purchase(unsigned price, unsigned qty) {
    ConsolListPurchaseUpdater cbUpdater(&cl);

    // No execution if we didn't insert inside the market
    if (offers.empty() || price < (--offers.end())->price) {
        return;
    }

    cbUpdater.onPurchaseBegin();

    // Iterate through the opposite half, in order of priority.
    auto it = offers.rbegin();
    while (it != offers.rend() && qty > 0 && it->price <= price) {
        // Subtract the executed quantities from the relevant variables.
        const unsigned exec_qty = min(it->displayedQty, qty);
        it->displayedQty -= exec_qty;
        it->qty -= exec_qty;
        qty -= exec_qty;
        cbUpdater.onMatch(*it);

        if (it->displayedQty == 0) {
            Offer iceberg = *it;
            it = OfferList::reverse_iterator(offers.erase(--it.base()));
            if (iceberg.replenish()) {
                // Insert with a new refnum so it loses priority.
                iceberg.oid.refNum = nextRefNum++;
                // Re-insert into the offers set at the end of the price level.
                auto new_it = offers.insert(move(iceberg)).first;
                cbUpdater.onReentry(*new_it);
                if (it == offers.rend() || it->price != iceberg.price)
                    it = OfferList::reverse_iterator(++new_it);
            }
        }
    }
    cbUpdater.onPurchaseEnd();
}


// Obtain a ConsolList only holding offers for a list with ID farmerID.
ConsolList subList(const ConsolList& cl, const char* farmerID) {
    ConsolList out;
    copy_if(cl.begin(), cl.end(), back_inserter(out),
                    [=] (const PublicOffer& po) {
        return strncmp(po.oid.farmerID.data(), farmerID, po.oid.farmerID.size()) == 0;
    });
    return out;
}
 
// Check if a ConsolList cl is identical in content to the offer list l.
bool isIdentical(const OfferList& l, const ConsolList& cl) {
    if (l.size() != cl.size()) {
        return false;
    }
    auto it1 = l.begin();
    auto it2 = cl.begin();
    for (; it1 != l.end() && it2 != cl.end(); ++it1, ++it2) {
        if (tie(it1->displayedQty, it1->price, it1->oid) != tie(it2->displayedQty, it2->price, it2->oid)) {
            return false;
        }
    }
    return true;
}

// Verify a ConsolList contains the correct list for the given MatchingEngine
bool verify(const ConsolList& cb, const MatchingEngine& m) {
    bool isValid = true;

    ConsolList subCB = subList(cb, m.getFarmerID());
    if (!is_sorted(cb.begin(), cb.end())) {
        cout << "Consolidated list is not sorted!" << endl;
        isValid = false;
    }
    if (!isIdentical(m.getList(), subCB)) {
        cout << "Mismatch between offer list and consolidated offer list.\n" 
                "Offers:\n";
        m.print();
        cout  << "\nExtract for farmer " << m.getFarmerID() << " from consolidated offers list: \n";
        print(subCB);
        isValid = false;
    }

    return isValid;
}
