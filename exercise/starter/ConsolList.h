// ************************
// IMPORTANT. You do not need to modify this file.
// ************************

#pragma once

#include "Base.h"

struct PublicOffer {
    unsigned price = 0;
    OfferID oid;
    unsigned displayedQty = 0;

    PublicOffer(const Offer& o) :
        price(o.price), oid(o.oid), displayedQty(o.displayedQty) {}
    PublicOffer() = default;

    // The better offer is 'larger'.
    bool operator<(const PublicOffer& r) const {
        // We prefer older entries when prices are equal
        if (price == r.price)
            return r.oid < oid;
        return r.price < price;
    }
};

// The offers are sorted by descending price, such that the the lowest
// offers reside at the end of the vector.
using ConsolList = vector<PublicOffer>;

// Helper to dump ConsolList to stdout
void print(const ConsolList& b);

// Obtain a ConsolList only holding offers for farmer farmerID.
ConsolList subList(const ConsolList& cb, const char* farmerID);

// Check if a ConsolList cb is identical in content to the list b.
bool isIdentical(const OfferList& b, const ConsolList& cb);

// Verify a ConsolList contains the correct offers for the given farmer's MatchingEngine
bool verify(const ConsolList& cb, const class MatchingEngine& m);
