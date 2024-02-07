#include <unordered_map>

#pragma once

#include "Base.h"

struct ConsolListEntryUpdater
{
    static void insert(ConsolList &cl, const Offer &o);
};

// Preconditions get from interviewer:
// 1. No concurrent usage of updaters.
// 2. `onPurchaseBegin` and `onPurchaseEnd` will always be called to guard
//    the usage of `onMatch` and `onReentry`.
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
    unordered_map<int, PublicOffer> bufferedOffers_;
};