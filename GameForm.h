#pragma once
#include "Card.h"
#include "Deck.h"
#include "Pile.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace System::Collections::Generic;

public ref class GameForm : public Form
{
private:
    Deck^ deck;
    List<Pile^>^ tableauPiles;
    List<Pile^>^ foundationPiles;
    Pile^ stockPile;
    Pile^ wastePile;

    // Drag & Drop переменные
    bool isDragging;
    Pile^ dragSourcePile;
    int dragStartIndex;
    Point dragStartPoint;
    List<Card^>^ dragCards;

    Button^ btnNewGame;
    Button^ btnDeal;
    Label^ lblStatus;

public:
    GameForm()
    {
        InitializeComponent();
        InitializeGame();
    }

private:
    void InitializeComponent()
    {
        this->Text = L"Пасьянс Косынка";
        this->Size = Drawing::Size(900, 700);
        this->StartPosition = FormStartPosition::CenterScreen;
        this->BackColor = Color::DarkGreen;
        this->DoubleBuffered = true;

        btnNewGame = gcnew Button();
        btnNewGame->Text = L"Новая игра";
        btnNewGame->Location = Point(10, 10);
        btnNewGame->Size = Drawing::Size(100, 30);
        btnNewGame->Click += gcnew EventHandler(this, &GameForm::btnNewGame_Click);
        this->Controls->Add(btnNewGame);

        btnDeal = gcnew Button();
        btnDeal->Text = L"Сдать карту";
        btnDeal->Location = Point(120, 10);
        btnDeal->Size = Drawing::Size(100, 30);
        btnDeal->Click += gcnew EventHandler(this, &GameForm::btnDeal_Click);
        this->Controls->Add(btnDeal);

        lblStatus = gcnew Label();
        lblStatus->Text = L"Добро пожаловать!";
        lblStatus->Location = Point(240, 15);
        lblStatus->Size = Drawing::Size(500, 20);
        lblStatus->ForeColor = Color::White;
        lblStatus->Font = gcnew System::Drawing::Font(L"Arial", 10.0f);
        this->Controls->Add(lblStatus);

        // Подключаем события мыши для Drag & Drop
        this->MouseDown += gcnew MouseEventHandler(this, &GameForm::GameForm_MouseDown);
        this->MouseMove += gcnew MouseEventHandler(this, &GameForm::GameForm_MouseMove);
        this->MouseUp += gcnew MouseEventHandler(this, &GameForm::GameForm_MouseUp);
        this->Paint += gcnew PaintEventHandler(this, &GameForm::GameForm_Paint);

        this->SetStyle(ControlStyles::UserPaint | ControlStyles::AllPaintingInWmPaint | ControlStyles::DoubleBuffer, true);
    }

    void InitializeGame()
    {
        try
        {
            deck = gcnew Deck();
            deck->Shuffle();

            tableauPiles = gcnew List<Pile^>();
            foundationPiles = gcnew List<Pile^>();

            stockPile = gcnew Pile(50, 80, false, false);
            wastePile = gcnew Pile(140, 80, false, false);

            for (int i = 0; i < 4; i++)
            {
                foundationPiles->Add(gcnew Pile(410 + i * 90, 80, true, false));
            }

            for (int i = 0; i < 7; i++)
            {
                Pile^ pile = gcnew Pile(50 + i * 90, 200, false, true);
                for (int j = 0; j <= i; j++)
                {
                    Card^ card = deck->DealCard();
                    if (card != nullptr)
                    {
                        card->SetFaceUp(j == i);
                        pile->AddCard(card);
                    }
                }
                tableauPiles->Add(pile);
            }

            while (!deck->IsEmpty())
            {
                Card^ card = deck->DealCard();
                if (card != nullptr)
                {
                    card->SetFaceUp(false);
                    stockPile->AddCard(card);
                }
            }

            isDragging = false;
            dragSourcePile = nullptr;
            dragStartIndex = -1;
            dragCards = nullptr;

            this->Invalidate();
            lblStatus->Text = L"Новая игра начата! Перетаскивайте карты мышкой";
        }
        catch (Exception^ ex)
        {
            MessageBox::Show("Ошибка: " + ex->Message);
        }
    }

    void GameForm_Paint(Object^ sender, PaintEventArgs^ e)
    {
        try
        {
            Graphics^ g = e->Graphics;

            if (stockPile != nullptr) stockPile->DrawPile(g);
            if (wastePile != nullptr) wastePile->DrawPile(g);

            if (foundationPiles != nullptr)
            {
                for each (Pile ^ pile in foundationPiles)
                {
                    if (pile != nullptr) pile->DrawPile(g);
                }
            }

            if (tableauPiles != nullptr)
            {
                for each (Pile ^ pile in tableauPiles)
                {
                    if (pile != nullptr) pile->DrawPile(g, 25);
                }
            }

            // Рисуем перетаскиваемые карты поверх всех
            if (isDragging && dragCards != nullptr && dragCards->Count > 0)
            {
                int x = dragStartPoint.X - 35; // Центрируем по курсору
                int y = dragStartPoint.Y - 50;

                for (int i = 0; i < dragCards->Count; i++)
                {
                    if (dragCards[i] != nullptr)
                    {
                        dragCards[i]->DrawCard(g, x, y + i * 25, 70, 100);
                    }
                }
            }
        }
        catch (Exception^) {}
    }

    void GameForm_MouseDown(Object^ sender, MouseEventArgs^ e)
    {
        // ИСПРАВЛЕНО: правильная проверка кнопки мыши
        if (e->Button != System::Windows::Forms::MouseButtons::Left)
            return;

        try
        {
            // Проверяем клик по игровым стопкам
            if (tableauPiles != nullptr)
            {
                for (int i = 0; i < tableauPiles->Count; i++)
                {
                    Pile^ pile = tableauPiles[i];
                    if (pile == nullptr) continue;

                    int cardIndex = pile->GetCardIndexAtPosition(e->X, e->Y);
                    if (cardIndex >= 0)
                    {
                        // Проверяем, все ли карты от cardIndex открыты
                        bool allOpen = true;
                        for (int j = cardIndex; j < pile->GetCards()->Count; j++)
                        {
                            if (pile->GetCards()[j] == nullptr || !pile->GetCards()[j]->IsFaceUp())
                            {
                                allOpen = false;
                                break;
                            }
                        }

                        if (!allOpen)
                        {
                            lblStatus->Text = L"Нельзя перетаскивать закрытые карты";
                            return;
                        }

                        // Проверяем последовательность
                        bool validSequence = true;
                        for (int j = cardIndex; j < pile->GetCards()->Count - 1; j++)
                        {
                            Card^ current = pile->GetCards()[j];
                            Card^ next = pile->GetCards()[j + 1];

                            if (current == nullptr || next == nullptr)
                            {
                                validSequence = false;
                                break;
                            }

                            if (current->GetRank() != next->GetRank() + 1 ||
                                current->GetCardColor() == next->GetCardColor())
                            {
                                validSequence = false;
                                break;
                            }
                        }

                        if (!validSequence)
                        {
                            lblStatus->Text = L"Карты должны идти по порядку";
                            return;
                        }

                        // Начинаем перетаскивание
                        isDragging = true;
                        dragSourcePile = pile;
                        dragStartIndex = cardIndex;

                        dragCards = gcnew List<Card^>();
                        for (int j = cardIndex; j < pile->GetCards()->Count; j++)
                        {
                            if (pile->GetCards()[j] != nullptr)
                                dragCards->Add(pile->GetCards()[j]);
                        }

                        dragStartPoint = e->Location;

                        lblStatus->Text = L"Перетаскиваю " + dragCards->Count.ToString() + L" карт";
                        this->Invalidate();
                        return;
                    }
                }
            }

            // Проверяем клик по сбросу
            if (wastePile != nullptr)
            {
                int cardIndex = wastePile->GetCardIndexAtPosition(e->X, e->Y);
                if (cardIndex >= 0)
                {
                    isDragging = true;
                    dragSourcePile = wastePile;
                    dragStartIndex = cardIndex;

                    dragCards = gcnew List<Card^>();
                    if (wastePile->GetCards()[cardIndex] != nullptr)
                        dragCards->Add(wastePile->GetCards()[cardIndex]);

                    dragStartPoint = e->Location;

                    lblStatus->Text = L"Перетаскиваю карту из сброса";
                    this->Invalidate();
                    return;
                }
            }
        }
        catch (Exception^ ex)
        {
            lblStatus->Text = L"Ошибка: " + ex->Message;
        }
    }

    void GameForm_MouseMove(Object^ sender, MouseEventArgs^ e)
    {
        if (isDragging && dragCards != nullptr)
        {
            dragStartPoint = e->Location;
            this->Invalidate();
        }
    }

    void GameForm_MouseUp(Object^ sender, MouseEventArgs^ e)
    {
        if (!isDragging || dragSourcePile == nullptr || dragCards == nullptr)
        {
            isDragging = false;
            return;
        }

        try
        {
            bool dropped = false;

            // Проверяем игровые стопки
            if (tableauPiles != nullptr)
            {
                for (int i = 0; i < tableauPiles->Count; i++)
                {
                    Pile^ pile = tableauPiles[i];
                    if (pile == nullptr || pile == dragSourcePile) continue;

                    if (pile->IsAtPosition(e->X, e->Y))
                    {
                        Card^ firstCard = dragCards[0];
                        if (firstCard == nullptr) break;

                        if (pile->IsEmpty() && firstCard->GetRank() == 13)
                        {
                            DropCards(dragSourcePile, pile, dragStartIndex);
                            dropped = true;
                            lblStatus->Text = L"Король перемещен";
                            break;
                        }
                        else if (!pile->IsEmpty() && pile->CanAddCard(firstCard))
                        {
                            DropCards(dragSourcePile, pile, dragStartIndex);
                            dropped = true;
                            lblStatus->Text = L"Карты перемещены";
                            break;
                        }
                        else
                        {
                            lblStatus->Text = L"Сюда нельзя положить эти карты";
                        }
                    }
                }
            }

            // Если не сбросили на игровую стопку, проверяем сборные стопки
            if (!dropped && foundationPiles != nullptr)
            {
                for (int i = 0; i < foundationPiles->Count; i++)
                {
                    Pile^ pile = foundationPiles[i];
                    if (pile == nullptr) continue;

                    if (pile->IsAtPosition(e->X, e->Y))
                    {
                        if (dragCards->Count > 1)
                        {
                            lblStatus->Text = L"В сборную стопку можно класть только по одной карте";
                            break;
                        }

                        Card^ card = dragCards[0];
                        if (card != nullptr && pile->CanAddCard(card))
                        {
                            // Забираем карту из источника
                            dragSourcePile->RemoveCard();
                            pile->AddCard(card);

                            if (!dragSourcePile->IsEmpty())
                            {
                                Card^ top = dragSourcePile->GetTopCard();
                                if (top != nullptr) top->SetFaceUp(true);
                            }

                            dropped = true;
                            CheckWin();
                            lblStatus->Text = L"Карта добавлена в сборную стопку";
                            break;
                        }
                        else
                        {
                            lblStatus->Text = L"Сюда нельзя положить эту карту";
                        }
                    }
                }
            }

            if (!dropped)
            {
                lblStatus->Text = L"Перемещение отменено";
            }

            if (!dragSourcePile->IsEmpty())
            {
                Card^ top = dragSourcePile->GetTopCard();
                if (top != nullptr) top->SetFaceUp(true);
            }
        }
        catch (Exception^ ex)
        {
            lblStatus->Text = L"Ошибка: " + ex->Message;
        }

        isDragging = false;
        dragSourcePile = nullptr;
        dragStartIndex = -1;
        dragCards = nullptr;
        this->Invalidate();
    }

    void DropCards(Pile^ source, Pile^ destination, int startIndex)
    {
        if (source == nullptr || destination == nullptr) return;
        if (source->GetCards() == nullptr || destination->GetCards() == nullptr) return;

        List<Card^>^ sourceCards = source->GetCards();

        // Создаем копию карт для перемещения
        List<Card^>^ cardsToMove = gcnew List<Card^>();
        for (int i = startIndex; i < sourceCards->Count; i++)
        {
            if (sourceCards[i] != nullptr)
                cardsToMove->Add(sourceCards[i]);
        }

        // Удаляем карты из источника (с конца)
        for (int i = sourceCards->Count - 1; i >= startIndex; i--)
        {
            sourceCards->RemoveAt(i);
        }

        // Добавляем карты в назначение
        for (int i = 0; i < cardsToMove->Count; i++)
        {
            if (cardsToMove[i] != nullptr)
                destination->AddCard(cardsToMove[i]);
        }

        // Открываем верхнюю карту в источнике
        if (!source->IsEmpty())
        {
            Card^ top = source->GetTopCard();
            if (top != nullptr) top->SetFaceUp(true);
        }

        CheckWin();
        this->Invalidate();
    }

    void HandleStockClick()
    {
        if (stockPile == nullptr) return;

        if (stockPile->IsEmpty())
        {
            if (wastePile != nullptr && !wastePile->IsEmpty())
            {
                List<Card^>^ wasteCards = wastePile->GetCards();
                while (wasteCards->Count > 0)
                {
                    Card^ card = wasteCards[wasteCards->Count - 1];
                    wasteCards->RemoveAt(wasteCards->Count - 1);
                    if (card != nullptr)
                    {
                        card->SetFaceUp(false);
                        stockPile->AddCard(card);
                    }
                }
                lblStatus->Text = L"Колода восстановлена";
                this->Invalidate();
            }
            else
            {
                lblStatus->Text = L"Колода и сброс пусты";
            }
            return;
        }

        Card^ card = stockPile->RemoveCard();
        if (card != nullptr && wastePile != nullptr)
        {
            card->SetFaceUp(true);
            wastePile->AddCard(card);
            lblStatus->Text = L"Сдана карта: " + card->GetRankString() + card->GetSuitSymbol();
            this->Invalidate();
        }
    }

    void CheckWin()
    {
        if (foundationPiles == nullptr) return;

        for each (Pile ^ pile in foundationPiles)
        {
            if (pile == nullptr || pile->GetSize() != 13)
                return;
        }

        MessageBox::Show("Поздравляем! Вы выиграли!", "Победа!",
            MessageBoxButtons::OK, MessageBoxIcon::Information);
        lblStatus->Text = L"Вы выиграли! 🎉";
    }

    void btnNewGame_Click(Object^ sender, EventArgs^ e)
    {
        InitializeGame();
    }

    void btnDeal_Click(Object^ sender, EventArgs^ e)
    {
        HandleStockClick();
    }
};
