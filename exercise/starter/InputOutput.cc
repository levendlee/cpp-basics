// ************************
// IMPORTANT. You will not need to read this. All the information you need should be in the document,
// and if you have questions, don't hesitate to ask your interviewer.
// This file solely contains print functions, so should be irrelevant to the task!
// ************************

#include "Base.h"
#include "ConsolList.h"

#include <iomanip>
#include <iostream>

ostream &operator<<(ostream &os, OfferID oid)
{
    ostringstream s;
    s << oid.refNum << ':' << oid.farmerID.data();
    return os << s.str();
}

void print(const OfferList &l)
{
    cout << right << setw(10) << "oid" << setw(10) << "qty" << setw(10) << "px"
         << "\n";
    for (const auto &o : l)
    {
        std::ostringstream str;
        str << o.displayedQty << "(" << o.qty << ")";
        cout << setw(10) << o.oid << setw(10) << str.str() << setw(10) << fixed << o.price << '\n';
    }
    cout << endl;
}

void print(const PublicOffer &po)
{
    cout << right << setw(10) << "oid" << setw(10) << "qty" << setw(10) << "px"
         << "\n";
    cout << setw(10) << po.oid << setw(10) << po.displayedQty << setw(10) << fixed << po.price << '\n';
}

void print(const ConsolList &cl)
{
    cout << right << setw(10) << "oid" << setw(10) << "qty" << setw(10) << "px"
         << "\n";
    for (const PublicOffer &po : cl)
    {
        cout << setw(10) << po.oid << setw(10) << po.displayedQty << setw(10) << fixed << po.price << '\n';
    }
    cout << endl;
}