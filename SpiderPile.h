#pragma once
#include "Card.h"

using namespace System;
using namespace System::Drawing;
using namespace System::Collections::Generic;

public ref class SpiderPile
{
private:
    List<Card^>^ cards;
    int x;
    int y;
    bool isFoundation;
    bool isTableau;

public:
    SpiderPile(int posX, int posY, bool foundation, bool tableau)
    {
        x = posX;
        y = posY;
        isFoundation = foundation;
        isTableau = tableau;
        cards = gcnew List<Card^>();
    }

    void AddCard(Card^ card)
    {
        if (cards != nullptr && card != nullptr)
            cards->Add(card);
    }

    Card^ RemoveCard()
    {
        if (cards == nullptr || cards->Count == 0) return nullptr;
        Card^ card = cards[cards->Count - 1];
        cards->RemoveAt(cards->Count - 1);

        // Если после удаления верхняя карта стала закрытой - открываем её
        if (cards->Count > 0 && !cards[cards->Count - 1]->IsFaceUp())
        {
            cards[cards->Count - 1]->SetFaceUp(true);
        }

        return card;
    }

    Card^ GetTopCard()
    {
        if (cards == nullptr || cards->Count == 0) return nullptr;
        return cards[cards->Count - 1];
    }

    List<Card^>^ GetCards()
    {
        return cards;
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

    bool CanAddCard(Card^ card)
    {
        if (cards == nullptr || card == nullptr) return false;

        if (isTableau)
        {
            if (cards->Count == 0)
                return true;  // В пауке на пустое место можно класть любую карту

            Card^ top = cards[cards->Count - 1];
            if (top == nullptr) return false;

            return card->GetRank() == top->GetRank() - 1;
        }

        return false;
    }

    Point GetCardPosition(int index)
    {
        if (index < 0 || index >= cards->Count)
            return Point(x, y);
        return Point(x, y + index * 25);
    }

    void DrawPile(Graphics^ g)
    {
        DrawPile(g, 25);
    }

    void DrawPile(Graphics^ g, int offset)
    {
        if (g == nullptr || cards == nullptr) return;

        if (cards->Count == 0)
        {
            try
            {
                Pen^ dashedPen = gcnew Pen(Color::Gray, 2.0f);
                dashedPen->DashStyle = System::Drawing::Drawing2D::DashStyle::Dash;

                float fx = static_cast<float>(x);
                float fy = static_cast<float>(y);
                g->DrawRectangle(dashedPen, fx, fy, 70.0f, 100.0f);
            }
            catch (Exception^) {}
            return;
        }

        for (int i = 0; i < cards->Count; i++)
        {
            try
            {
                Card^ card = cards[i];
                if (card == nullptr) continue;

                int yOffset = (isTableau) ? i * offset : 0;

                card->DrawCard(g, x, y + yOffset, 70, 100);
            }
            catch (Exception^) {}
        }
    }

    bool IsAtPosition(int mouseX, int mouseY)
    {
        if (cards == nullptr) return false;

        int height = (isTableau && cards->Count > 0) ?
            (cards->Count - 1) * 25 + 100 : 100;

        return mouseX >= x && mouseX <= x + 70 &&
            mouseY >= y && mouseY <= y + height;
    }

    int GetCardIndexAtPosition(int mouseX, int mouseY)
    {
        if (cards == nullptr || cards->Count == 0) return -1;
        if (!IsAtPosition(mouseX, mouseY)) return -1;

        if (isTableau)
        {
            int index = (mouseY - y) / 25;
            if (index >= cards->Count) index = cards->Count - 1;

            if (index >= 0 && index < cards->Count)
            {
                Card^ card = cards[index];
                if (card != nullptr && card->IsFaceUp())
                    return index;
            }
        }

        return -1;
    }

    void SetPosition(int newX, int newY)
    {
        x = newX;
        y = newY;
    }

    int GetX() { return x; }
    int GetY() { return y; }
};