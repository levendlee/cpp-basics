#include <unordered_map>

#pragma once

#include "Base.h"

struct ConsolListEntryUpdater
{
    static void insert(ConsolList &cl, const Offer &o);
};

// No concurrent updaters.
class ConsolListPurchaseUpdater
{
    ConsolList *cl = nullptr;

public:
    ConsolListPurchaseUpdater(ConsolList *p) : cl(p) {}

    void onPurchaseBegin();
    void onMatch(const Offer &offer);
    void onReentry(const Offer &icebergOffer);
    void onPurchaseEnd();

private:
    unorderded_map<int, PublicOffer> bufferedOffers_;
};