#pragma once
#include "Card.h"

using namespace System;
using namespace System::Collections::Generic;

public ref class SpiderDeck
{
private:
    List<Card^>^ cards;
    static Random^ rand = gcnew Random();

public:
    SpiderDeck(int suits)
    {
        cards = gcnew List<Card^>();

        // СОЗДАЕМ 104 КАРТЫ (2 КОЛОДЫ ПО 52 КАРТЫ)
        // Для 1 масти используем только ПИКИ (♠)
        for (int deck = 0; deck < 2; deck++)  // 2 колоды
        {
            // Для 1 масти: 4 масти × 13 рангов = 52 карты в колоде
            for (int suit = 0; suit < 4; suit++)  // 4 масти
            {
                for (int rank = 1; rank <= 13; rank++)  // 13 рангов
                {
                    Card^ card;
                    if (suits == 1)
                    {
                        // Только пики
                        card = gcnew Card(Suit::Spades, rank);
                    }
                    else if (suits == 2)
                    {
                        // Пики и черви
                        if (suit < 2)
                            card = gcnew Card(Suit::Spades, rank);
                        else
                            card = gcnew Card(Suit::Hearts, rank);
                    }
                    else
                    {
                        // Все 4 масти
                        card = gcnew Card((Suit)suit, rank);
                    }
                    cards->Add(card);
                }
            }
        }

        // Перемешиваем
        Shuffle();
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
        if (cards == nullptr || cards->Count == 0)
        {
            return nullptr;
        }
        Card^ card = cards[cards->Count - 1];
        cards->RemoveAt(cards->Count - 1);
        return card;
    }

    int GetSize()
    {
        if (cards == nullptr) return 0;
        return cards->Count;
    }

    bool IsEmpty()
    {
        if (cards == nullptr) return true;
        return cards->Count == 0;
    }
};