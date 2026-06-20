#pragma once

using namespace System;
using namespace System::Drawing;

public enum class Suit { Hearts, Diamonds, Clubs, Spades };
public enum class CardColor { Red, Black };

public ref class Card
{
private:
    Suit suit;
    int rank;
    bool faceUp;

public:
    Card(Suit s, int r)
    {
        suit = s;
        rank = r;
        faceUp = false;
    }

    Suit GetSuit() { return suit; }
    int GetRank() { return rank; }
    bool IsFaceUp() { return faceUp; }
    void SetFaceUp(bool up) { faceUp = up; }

    CardColor GetCardColor()
    {
        return (suit == Suit::Hearts || suit == Suit::Diamonds) ? CardColor::Red : CardColor::Black;
    }

    String^ GetRankString()
    {
        if (rank == 1) return "A";
        if (rank == 11) return "J";
        if (rank == 12) return "Q";
        if (rank == 13) return "K";
        return rank.ToString();
    }

    String^ GetSuitSymbol()
    {
        if (suit == Suit::Hearts) return "♥";
        if (suit == Suit::Diamonds) return "♦";
        if (suit == Suit::Clubs) return "♣";
        return "♠";
    }

    void DrawCard(Graphics^ g, int x, int y, int width, int height)
    {
        if (g == nullptr) return;

        try
        {
            float fx = static_cast<float>(x);
            float fy = static_cast<float>(y);
            float fwidth = static_cast<float>(width);
            float fheight = static_cast<float>(height);

            SolidBrush^ whiteBrush = gcnew SolidBrush(Color::White);
            SolidBrush^ blackBrush = gcnew SolidBrush(Color::Black);
            Pen^ blackPen = gcnew Pen(Color::Black, 2.0f);

            g->FillRectangle(whiteBrush, fx, fy, fwidth, fheight);
            g->DrawRectangle(blackPen, fx, fy, fwidth, fheight);

            if (faceUp)
            {
                Color color = (GetCardColor() == CardColor::Red) ? Color::Red : Color::Black;
                SolidBrush^ textBrush = gcnew SolidBrush(color);

                System::Drawing::Font^ font = gcnew System::Drawing::Font(L"Arial", 14.0f, FontStyle::Bold);

                String^ text = GetRankString() + GetSuitSymbol();
                g->DrawString(text, font, textBrush, fx + 5.0f, fy + 5.0f);

                System::Drawing::Font^ bigFont = gcnew System::Drawing::Font(L"Arial", 24.0f, FontStyle::Bold);
                float centerX = fx + fwidth / 2.0f - 15.0f;
                float centerY = fy + fheight / 2.0f - 20.0f;
                g->DrawString(GetSuitSymbol(), bigFont, textBrush, centerX, centerY);

                String^ bottomText = GetRankString() + GetSuitSymbol();
                g->DrawString(bottomText, font, textBrush, fx + fwidth - 40.0f, fy + fheight - 30.0f);
            }
            else
            {
                SolidBrush^ backBrush = gcnew SolidBrush(Color::DarkBlue);
                g->FillRectangle(backBrush, fx + 2.0f, fy + 2.0f, fwidth - 4.0f, fheight - 4.0f);

                Pen^ patternPen = gcnew Pen(Color::LightBlue, 1.0f);
                for (float i = 0; i < fwidth; i += 10.0f)
                {
                    g->DrawLine(patternPen, fx + i, fy, fx + i, fy + fheight);
                }
                for (float i = 0; i < fheight; i += 10.0f)
                {
                    g->DrawLine(patternPen, fx, fy + i, fx + fwidth, fy + i);
                }
            }
        }
        catch (Exception^) {}
    }
};