#include "Base.h"
#include "ConsolList.h"

int main()
{
    // Here, you can test your updater implementations.
    // In each test case, a MatchingEngine will contain an offer list and you can enter offers via
    // .addOffer(Offer), where Offer is conveniently initialized by its aggregate initializer.
    // You can also use the auxiliary function verify, which checks if a ConsolList correctly contains
    // all offers for a given MatchingEngine, and alerts otherwise.
    // Finally, you can print the current offer list of a matching engine "me" by "me.print()".
    // See below for some examples.

    ConsolList cb;
    {
        MatchingEngine me("A", cb);
        me.addOffer({1, 10, 5}); // Iceberg with price $1, total apples 10 and 5 visible
        me.addOffer({1, 3});     // Regular offer with price $1, and 3 apples
        me.purchase(1, 10);      // Purchase 10 apples for no more than $1 each
        verify(cb, me);
    }
    {
        MatchingEngine me("B", cb);
        me.addOffer({1, 5});
        me.addOffer({1, 10});
        me.purchase(1, 11);
        verify(cb, me);
    }
    cout << "\nConsolidated list: \n";    print(cb);
}
