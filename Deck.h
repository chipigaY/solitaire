#pragma once
#include "Card.h"

using namespace System;
using namespace System::Collections::Generic;

public ref class Deck
{
private:
    List<Card^>^ cards;
    static Random^ rand = gcnew Random();

public:
    Deck()
    {
        cards = gcnew List<Card^>();
        InitializeDeck();
    }

    void InitializeDeck()
    {
        if (cards == nullptr) return;
        cards->Clear();

        for (int suit = 0; suit < 4; suit++)
        {
            for (int rank = 1; rank <= 13; rank++)
            {
                cards->Add(gcnew Card(static_cast<Suit>(suit), rank));
            }
        }
    }

    void Shuffle()
    {
        if (cards == nullptr) return;

        for (int i = cards->Count - 1; i > 0; i--)
        {
            int j = rand->Next(i + 1);
            Card^ temp = cards[i];
            cards[i] = cards[j];
            cards[j] = temp;
        }
    }

    Card^ DealCard()
    {
        if (cards == nullptr || cards->Count == 0) return nullptr;
        Card^ card = cards[cards->Count - 1];
        cards->RemoveAt(cards->Count - 1);
        return card;
    }

    int GetSize()
    {
        if (cards == nullptr) return 0;
        return cards->Count;
    }

    void AddCard(Card^ card)
    {
        if (cards != nullptr && card != nullptr)
            cards->Add(card);
    }

    bool IsEmpty()
    {
        if (cards == nullptr) return true;
        return cards->Count == 0;
    }
};