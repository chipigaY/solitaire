#pragma once
#include "Card.h"
#include "SpiderDeck.h"
#include "SpiderPile.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace System::Collections::Generic;

public ref class SpiderGameForm : public Form
{
private:
    SpiderDeck^ deck;
    List<SpiderPile^>^ tableauPiles;

    // Drag & Drop переменные
    bool isDragging;
    SpiderPile^ dragSourcePile;
    int dragStartIndex;
    Point dragStartPoint;
    List<Card^>^ dragCards;
    Point dragOffset;

    // Настройки игры
    int suitsCount;
    int movesCount;
    int completedSuits;
    int dealsRemaining;  // Сколько раздач осталось
    bool gameOver;

    // UI элементы
    Button^ btnNewGame;
    Button^ btnDeal;
    Button^ btnBack;
    Label^ lblStatus;
    Label^ lblMoves;
    Label^ lblCompleted;
    Label^ lblCardsLeft;
    Label^ lblDealsLeft;

public:
    SpiderGameForm(int suits)
    {
        suitsCount = suits;
        InitializeComponent();
        InitializeGame();
    }

private:
    void InitializeComponent()
    {
        this->Text = L"Пасьянс Паук - 1 масть";
        this->Size = Drawing::Size(950, 750);
        this->StartPosition = FormStartPosition::CenterScreen;
        this->BackColor = Color::DarkGreen;
        this->DoubleBuffered = true;
        this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
        this->MaximizeBox = true;
        this->MinimizeBox = true;

        btnBack = gcnew Button();
        btnBack->Text = L"← Меню";
        btnBack->Location = Point(10, 10);
        btnBack->Size = Drawing::Size(80, 30);
        btnBack->Click += gcnew EventHandler(this, &SpiderGameForm::btnBack_Click);
        this->Controls->Add(btnBack);

        btnNewGame = gcnew Button();
        btnNewGame->Text = L"Новая игра";
        btnNewGame->Location = Point(100, 10);
        btnNewGame->Size = Drawing::Size(100, 30);
        btnNewGame->Click += gcnew EventHandler(this, &SpiderGameForm::btnNewGame_Click);
        this->Controls->Add(btnNewGame);

        btnDeal = gcnew Button();
        btnDeal->Text = L"Сдать карты";
        btnDeal->Location = Point(210, 10);
        btnDeal->Size = Drawing::Size(100, 30);
        btnDeal->Click += gcnew EventHandler(this, &SpiderGameForm::btnDeal_Click);
        this->Controls->Add(btnDeal);

        lblStatus = gcnew Label();
        lblStatus->Text = L"Добро пожаловать в Паук!";
        lblStatus->Location = Point(330, 15);
        lblStatus->Size = Drawing::Size(350, 20);
        lblStatus->ForeColor = Color::White;
        lblStatus->Font = gcnew System::Drawing::Font(L"Arial", 10.0f);
        this->Controls->Add(lblStatus);

        lblMoves = gcnew Label();
        lblMoves->Text = L"Ходов: 0";
        lblMoves->Location = Point(690, 15);
        lblMoves->Size = Drawing::Size(80, 20);
        lblMoves->ForeColor = Color::White;
        lblMoves->Font = gcnew System::Drawing::Font(L"Arial", 10.0f);
        this->Controls->Add(lblMoves);

        lblCompleted = gcnew Label();
        lblCompleted->Text = L"Собрано: 0/8";
        lblCompleted->Location = Point(780, 15);
        lblCompleted->Size = Drawing::Size(90, 20);
        lblCompleted->ForeColor = Color::White;
        lblCompleted->Font = gcnew System::Drawing::Font(L"Arial", 10.0f);
        this->Controls->Add(lblCompleted);

        lblCardsLeft = gcnew Label();
        lblCardsLeft->Text = L"В колоде: 50";
        lblCardsLeft->Location = Point(880, 15);
        lblCardsLeft->Size = Drawing::Size(90, 20);
        lblCardsLeft->ForeColor = Color::White;
        lblCardsLeft->Font = gcnew System::Drawing::Font(L"Arial", 10.0f);
        this->Controls->Add(lblCardsLeft);

        lblDealsLeft = gcnew Label();
        lblDealsLeft->Text = L"Раздач: 5";
        lblDealsLeft->Location = Point(970, 15);
        lblDealsLeft->Size = Drawing::Size(80, 20);
        lblDealsLeft->ForeColor = Color::White;
        lblDealsLeft->Font = gcnew System::Drawing::Font(L"Arial", 10.0f);
        this->Controls->Add(lblDealsLeft);

        this->MouseDown += gcnew MouseEventHandler(this, &SpiderGameForm::GameForm_MouseDown);
        this->MouseMove += gcnew MouseEventHandler(this, &SpiderGameForm::GameForm_MouseMove);
        this->MouseUp += gcnew MouseEventHandler(this, &SpiderGameForm::GameForm_MouseUp);
        this->Paint += gcnew PaintEventHandler(this, &SpiderGameForm::GameForm_Paint);

        this->SetStyle(ControlStyles::UserPaint | ControlStyles::AllPaintingInWmPaint | ControlStyles::DoubleBuffer, true);

        isDragging = false;
        dragSourcePile = nullptr;
        dragStartIndex = -1;
        dragCards = nullptr;
        dragOffset = Point(0, 0);
    }

    void InitializeGame()
    {
        try
        {
            movesCount = 0;
            completedSuits = 0;
            dealsRemaining = 5;  // 5 раздач по 10 карт
            gameOver = false;

            ResetDragState();

            // Создаем колоду из 104 карт (2 колоды по 52)
            deck = gcnew SpiderDeck(suitsCount);
            deck->Shuffle();

            // Создаем 10 игровых стопок
            tableauPiles = gcnew List<SpiderPile^>();

            for (int i = 0; i < 10; i++)
            {
                int xPos = 15 + i * 75;
                SpiderPile^ pile = gcnew SpiderPile(xPos, 170, false, true);
                tableauPiles->Add(pile);
            }

            // Раздаем начальные карты: первые 4 стопки по 6 карт, остальные 6 по 5 карт
            // ВСЕГО: 4*6 + 6*5 = 24 + 30 = 54 карты
            int totalDealt = 0;
            for (int i = 0; i < 10; i++)
            {
                int cardsCount = (i < 4) ? 6 : 5;
                for (int j = 0; j < cardsCount; j++)
                {
                    Card^ card = deck->DealCard();
                    if (card != nullptr)
                    {
                        // Все карты, кроме последней в стопке, закрыты
                        card->SetFaceUp(j == cardsCount - 1);
                        tableauPiles[i]->AddCard(card);
                        totalDealt++;
                    }
                }
            }

            UpdateStats();
            this->Invalidate();

            lblStatus->Text = L"Новая игра! Раздано: " + totalDealt.ToString() +
                L" карт, в колоде: " + deck->GetSize().ToString() +
                L" карт. Осталось раздач: " + dealsRemaining.ToString();
        }
        catch (Exception^ ex)
        {
            MessageBox::Show("Ошибка: " + ex->Message);
        }
    }

    void ResetDragState()
    {
        isDragging = false;
        dragSourcePile = nullptr;
        dragStartIndex = -1;
        if (dragCards != nullptr)
        {
            dragCards->Clear();
            delete dragCards;
            dragCards = nullptr;
        }
        dragOffset = Point(0, 0);
    }

    void GameForm_Paint(Object^ sender, PaintEventArgs^ e)
    {
        try
        {
            Graphics^ g = e->Graphics;

            // Информация о колоде
            if (deck != nullptr)
            {
                String^ info = L"Колода: " + deck->GetSize().ToString() + L" карт";
                System::Drawing::Font^ infoFont = gcnew System::Drawing::Font(L"Arial", 10.0f, FontStyle::Bold);
                SolidBrush^ infoBrush = gcnew SolidBrush(Color::White);
                g->DrawString(info, infoFont, infoBrush, 15.0f, 145.0f);
            }

            // Рисуем все стопки
            if (tableauPiles != nullptr)
            {
                for each (SpiderPile ^ pile in tableauPiles)
                {
                    if (pile != nullptr) pile->DrawPile(g, 25);
                }
            }

            // Рисуем перетаскиваемые карты
            if (isDragging && dragCards != nullptr && dragCards->Count > 0)
            {
                int x = dragStartPoint.X - dragOffset.X;
                int y = dragStartPoint.Y - dragOffset.Y;

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
        if (e->Button != System::Windows::Forms::MouseButtons::Left)
            return;

        if (gameOver)
        {
            lblStatus->Text = L"Игра окончена. Начните новую игру.";
            return;
        }

        if (isDragging)
            return;

        try
        {
            if (tableauPiles != nullptr)
            {
                for (int i = 0; i < tableauPiles->Count; i++)
                {
                    SpiderPile^ pile = tableauPiles[i];
                    if (pile == nullptr) continue;

                    int cardIndex = pile->GetCardIndexAtPosition(e->X, e->Y);
                    if (cardIndex >= 0)
                    {
                        // Проверяем, все ли карты открыты
                        bool allOpen = true;
                        for (int j = cardIndex; j < pile->GetCards()->Count; j++)
                        {
                            if (!pile->GetCards()[j]->IsFaceUp())
                            {
                                allOpen = false;
                                break;
                            }
                        }

                        if (!allOpen)
                        {
                            lblStatus->Text = L"Можно перетаскивать только открытые карты";
                            return;
                        }

                        // Проверяем последовательность (по убыванию)
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

                            if (current->GetRank() != next->GetRank() + 1)
                            {
                                validSequence = false;
                                break;
                            }
                        }

                        if (!validSequence)
                        {
                            lblStatus->Text = L"Карты должны идти по убыванию";
                            return;
                        }

                        isDragging = true;
                        dragSourcePile = pile;
                        dragStartIndex = cardIndex;

                        dragCards = gcnew List<Card^>();
                        for (int j = cardIndex; j < pile->GetCards()->Count; j++)
                        {
                            if (pile->GetCards()[j] != nullptr)
                                dragCards->Add(pile->GetCards()[j]);
                        }

                        Point cardPos = pile->GetCardPosition(cardIndex);
                        dragOffset = Point(e->X - cardPos.X, e->Y - cardPos.Y);
                        dragStartPoint = e->Location;

                        lblStatus->Text = L"Перетаскиваю " + dragCards->Count.ToString() + L" карт";
                        this->Invalidate();
                        return;
                    }
                }
            }
        }
        catch (Exception^ ex)
        {
            lblStatus->Text = L"Ошибка: " + ex->Message;
            ResetDragState();
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
            ResetDragState();
            this->Invalidate();
            return;
        }

        try
        {
            bool dropped = false;

            if (tableauPiles != nullptr)
            {
                for (int i = 0; i < tableauPiles->Count; i++)
                {
                    SpiderPile^ pile = tableauPiles[i];
                    if (pile == nullptr || pile == dragSourcePile) continue;

                    if (pile->IsAtPosition(e->X, e->Y))
                    {
                        Card^ firstCard = dragCards[0];
                        if (firstCard == nullptr) break;

                        if (pile->IsEmpty() || pile->CanAddCard(firstCard))
                        {
                            List<Card^>^ sourceCards = dragSourcePile->GetCards();
                            List<Card^>^ destCards = pile->GetCards();

                            // Удаляем карты из источника
                            for (int j = 0; j < dragCards->Count; j++)
                            {
                                sourceCards->RemoveAt(sourceCards->Count - 1);
                            }

                            // Добавляем карты в назначение
                            for each (Card ^ card in dragCards)
                            {
                                destCards->Add(card);
                            }

                            // Открываем верхнюю карту в источнике, если она закрыта
                            if (!dragSourcePile->IsEmpty())
                            {
                                Card^ top = dragSourcePile->GetTopCard();
                                if (top != nullptr && !top->IsFaceUp())
                                    top->SetFaceUp(true);
                            }

                            dropped = true;
                            movesCount++;
                            lblStatus->Text = L"Карты перемещены";

                            CheckAndRemoveCompleteSequences();
                            UpdateStats();
                            this->Invalidate();
                            break;
                        }
                        else
                        {
                            lblStatus->Text = L"Сюда нельзя положить эти карты";
                        }
                    }
                }
            }

            if (!dropped)
            {
                lblStatus->Text = L"Перемещение отменено";
            }
        }
        catch (Exception^ ex)
        {
            lblStatus->Text = L"Ошибка: " + ex->Message;
        }

        ResetDragState();
        this->Invalidate();
    }

    void CheckAndRemoveCompleteSequences()
    {
        if (tableauPiles == nullptr) return;

        bool sequenceFound = true;
        while (sequenceFound)
        {
            sequenceFound = false;

            for each (SpiderPile ^ pile in tableauPiles)
            {
                if (pile == nullptr || pile->GetSize() < 13) continue;

                List<Card^>^ cards = pile->GetCards();
                int startIndex = cards->Count - 13;

                if (startIndex >= 0)
                {
                    bool isComplete = true;
                    for (int i = 0; i < 13; i++)
                    {
                        Card^ card = cards[startIndex + i];
                        if (card == nullptr || !card->IsFaceUp() || card->GetRank() != 13 - i)
                        {
                            isComplete = false;
                            break;
                        }
                    }

                    if (isComplete)
                    {
                        // Удаляем полную последовательность
                        for (int i = 0; i < 13; i++)
                        {
                            cards->RemoveAt(cards->Count - 1);
                        }

                        completedSuits++;
                        sequenceFound = true;
                        lblStatus->Text = L"Полная последовательность удалена! (" + completedSuits.ToString() + L"/8)";

                        // Открываем верхнюю карту, если она закрыта
                        if (!pile->IsEmpty())
                        {
                            Card^ top = pile->GetTopCard();
                            if (top != nullptr && !top->IsFaceUp())
                                top->SetFaceUp(true);
                        }

                        CheckWin();
                        this->Invalidate();
                        break;
                    }
                }
            }
        }
    }

    void HandleDealClick()
    {
        if (gameOver)
        {
            lblStatus->Text = L"Игра окончена. Начните новую игру.";
            return;
        }

        if (deck == nullptr)
        {
            lblStatus->Text = L"Ошибка: колода не инициализирована";
            return;
        }

        if (deck->IsEmpty())
        {
            lblStatus->Text = L"Колода пуста!";
            CheckForPossibleMoves();
            return;
        }

        if (dealsRemaining <= 0)
        {
            lblStatus->Text = L"Больше нет раздач!";
            CheckForPossibleMoves();
            return;
        }

        // Проверяем, есть ли пустые стопки
        bool hasEmptyPile = false;
        for each (SpiderPile ^ pile in tableauPiles)
        {
            if (pile != nullptr && pile->IsEmpty())
            {
                hasEmptyPile = true;
                break;
            }
        }

        if (hasEmptyPile)
        {
            lblStatus->Text = L"Нельзя сдавать карты, есть пустые стопки!";
            MessageBox::Show("Заполните все пустые стопки перед сдачей карт!",
                "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Warning);
            return;
        }

        // Сдаем по 1 карте на каждую из 10 стопок (всего 10 карт)
        int cardsDealt = 0;
        for each (SpiderPile ^ pile in tableauPiles)
        {
            if (pile == nullptr) continue;

            if (!deck->IsEmpty())
            {
                Card^ card = deck->DealCard();
                if (card != nullptr)
                {
                    card->SetFaceUp(true);  // Сдаваемые карты всегда открыты
                    pile->AddCard(card);
                    cardsDealt++;
                }
            }
        }

        if (cardsDealt > 0)
        {
            dealsRemaining--;
            movesCount++;
            lblStatus->Text = L"Сдано " + cardsDealt.ToString() + L" карт. Осталось раздач: " + dealsRemaining.ToString();

            CheckAndRemoveCompleteSequences();
            UpdateStats();
            this->Invalidate();
        }
        else
        {
            lblStatus->Text = L"Нет карт для сдачи";
        }
    }

    void CheckForPossibleMoves()
    {
        bool hasMoves = false;

        if (tableauPiles != nullptr)
        {
            for (int i = 0; i < tableauPiles->Count && !hasMoves; i++)
            {
                SpiderPile^ sourcePile = tableauPiles[i];
                if (sourcePile == nullptr || sourcePile->IsEmpty()) continue;

                List<Card^>^ sourceCards = sourcePile->GetCards();

                for (int k = 0; k < sourceCards->Count && !hasMoves; k++)
                {
                    Card^ sourceCard = sourceCards[k];
                    if (sourceCard == nullptr || !sourceCard->IsFaceUp()) continue;

                    for (int j = 0; j < tableauPiles->Count && !hasMoves; j++)
                    {
                        if (j == i) continue;

                        SpiderPile^ destPile = tableauPiles[j];
                        if (destPile == nullptr) continue;

                        if (destPile->IsEmpty() || destPile->CanAddCard(sourceCard))
                        {
                            hasMoves = true;
                        }
                    }
                }
            }
        }

        if (!hasMoves && deck != nullptr && deck->IsEmpty() && dealsRemaining <= 0)
        {
            gameOver = true;
            lblStatus->Text = L"Игра проиграна! Нет возможных ходов.";
            MessageBox::Show("Нет возможных ходов! Игра проиграна.", "Поражение",
                MessageBoxButtons::OK, MessageBoxIcon::Information);
        }
    }

    void CheckWin()
    {
        if (completedSuits == 8)
        {
            gameOver = true;
            MessageBox::Show("Поздравляем! Вы выиграли!", "Победа!",
                MessageBoxButtons::OK, MessageBoxIcon::Information);
            lblStatus->Text = L"Вы выиграли! 🎉";
        }
    }

    void UpdateStats()
    {
        lblMoves->Text = L"Ходов: " + movesCount.ToString();
        lblCompleted->Text = L"Собрано: " + completedSuits.ToString() + L"/8";
        if (deck != nullptr)
            lblCardsLeft->Text = L"В колоде: " + deck->GetSize().ToString();
        lblDealsLeft->Text = L"Раздач: " + dealsRemaining.ToString();
    }

    void btnNewGame_Click(Object^ sender, EventArgs^ e)
    {
        if (MessageBox::Show("Начать новую игру?", "Подтверждение",
            MessageBoxButtons::YesNo, MessageBoxIcon::Question) == System::Windows::Forms::DialogResult::Yes)
        {
            InitializeGame();
        }
    }

    void btnDeal_Click(Object^ sender, EventArgs^ e)
    {
        HandleDealClick();
    }

    void btnBack_Click(Object^ sender, EventArgs^ e)
    {
        this->Close();
    }
};