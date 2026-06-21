#pragma once
#include "GameForm.h"
#include "SpiderGameForm.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;

public ref class MainMenuForm : public Form
{
private:
    Label^ lblTitle;
    Button^ btnKlondike;
    Button^ btnSpider;
    Button^ btnExit;
    Panel^ panelTitle;
    Label^ lblSubtitle;

public:
    MainMenuForm()
    {
        InitializeComponent();
    }

private:
    void InitializeComponent()
    {
        this->Text = L"Пасьянсы";
        this->Size = Drawing::Size(500, 450);
        this->StartPosition = FormStartPosition::CenterScreen;
        this->BackColor = Color::FromArgb(0, 100, 0);
        this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
        this->MaximizeBox = false;
        this->MinimizeBox = false;

        panelTitle = gcnew Panel();
        panelTitle->Location = Point(0, 0);
        panelTitle->Size = Drawing::Size(500, 120);
        panelTitle->BackColor = Color::FromArgb(0, 70, 0);

        lblTitle = gcnew Label();
        lblTitle->Text = L"♠ ♥ ♣ ♦";
        lblTitle->Location = Point(150, 15);
        lblTitle->Size = Drawing::Size(200, 40);
        lblTitle->Font = gcnew System::Drawing::Font(L"Arial", 28.0f, FontStyle::Bold);
        lblTitle->ForeColor = Color::Gold;
        lblTitle->TextAlign = ContentAlignment::MiddleCenter;
        panelTitle->Controls->Add(lblTitle);

        lblSubtitle = gcnew Label();
        lblSubtitle->Text = L"Выберите игру";
        lblSubtitle->Location = Point(150, 65);
        lblSubtitle->Size = Drawing::Size(200, 30);
        lblSubtitle->Font = gcnew System::Drawing::Font(L"Arial", 16.0f);
        lblSubtitle->ForeColor = Color::White;
        lblSubtitle->TextAlign = ContentAlignment::MiddleCenter;
        panelTitle->Controls->Add(lblSubtitle);

        this->Controls->Add(panelTitle);

        btnKlondike = gcnew Button();
        btnKlondike->Text = L"♣ Косынка (Klondike)";
        btnKlondike->Location = Point(75, 150);
        btnKlondike->Size = Drawing::Size(350, 60);
        btnKlondike->Font = gcnew System::Drawing::Font(L"Arial", 16.0f, FontStyle::Bold);
        btnKlondike->BackColor = Color::FromArgb(0, 80, 0);
        btnKlondike->ForeColor = Color::White;
        btnKlondike->FlatStyle = FlatStyle::Flat;
        btnKlondike->FlatAppearance->BorderSize = 2;
        btnKlondike->FlatAppearance->BorderColor = Color::Gold;
        btnKlondike->Cursor = Cursors::Hand;
        btnKlondike->Click += gcnew EventHandler(this, &MainMenuForm::btnKlondike_Click);
        btnKlondike->MouseEnter += gcnew EventHandler(this, &MainMenuForm::btn_MouseEnter);
        btnKlondike->MouseLeave += gcnew EventHandler(this, &MainMenuForm::btn_MouseLeave);
        this->Controls->Add(btnKlondike);

        btnSpider = gcnew Button();
        btnSpider->Text = L"🕷 Паук (Spider) - 1 масть";
        btnSpider->Location = Point(75, 230);
        btnSpider->Size = Drawing::Size(350, 60);
        btnSpider->Font = gcnew System::Drawing::Font(L"Arial", 16.0f, FontStyle::Bold);
        btnSpider->BackColor = Color::FromArgb(0, 80, 0);
        btnSpider->ForeColor = Color::White;
        btnSpider->FlatStyle = FlatStyle::Flat;
        btnSpider->FlatAppearance->BorderSize = 2;
        btnSpider->FlatAppearance->BorderColor = Color::Gold;
        btnSpider->Cursor = Cursors::Hand;
        btnSpider->Click += gcnew EventHandler(this, &MainMenuForm::btnSpider_Click);
        btnSpider->MouseEnter += gcnew EventHandler(this, &MainMenuForm::btn_MouseEnter);
        btnSpider->MouseLeave += gcnew EventHandler(this, &MainMenuForm::btn_MouseLeave);
        this->Controls->Add(btnSpider);

        btnExit = gcnew Button();
        btnExit->Text = L"✕ Выход";
        btnExit->Location = Point(175, 320);
        btnExit->Size = Drawing::Size(150, 40);
        btnExit->Font = gcnew System::Drawing::Font(L"Arial", 12.0f);
        btnExit->BackColor = Color::FromArgb(80, 0, 0);
        btnExit->ForeColor = Color::White;
        btnExit->FlatStyle = FlatStyle::Flat;
        btnExit->FlatAppearance->BorderSize = 1;
        btnExit->FlatAppearance->BorderColor = Color::DarkRed;
        btnExit->Cursor = Cursors::Hand;
        btnExit->Click += gcnew EventHandler(this, &MainMenuForm::btnExit_Click);
        btnExit->MouseEnter += gcnew EventHandler(this, &MainMenuForm::btnExit_MouseEnter);
        btnExit->MouseLeave += gcnew EventHandler(this, &MainMenuForm::btnExit_MouseLeave);
        this->Controls->Add(btnExit);
    }

    void btn_MouseEnter(Object^ sender, EventArgs^ e)
    {
        Button^ btn = safe_cast<Button^>(sender);
        btn->BackColor = Color::FromArgb(0, 120, 0);
        btn->FlatAppearance->BorderColor = Color::White;
    }

    void btn_MouseLeave(Object^ sender, EventArgs^ e)
    {
        Button^ btn = safe_cast<Button^>(sender);
        btn->BackColor = Color::FromArgb(0, 80, 0);
        btn->FlatAppearance->BorderColor = Color::Gold;
    }

    void btnExit_MouseEnter(Object^ sender, EventArgs^ e)
    {
        Button^ btn = safe_cast<Button^>(sender);
        btn->BackColor = Color::FromArgb(120, 0, 0);
    }

    void btnExit_MouseLeave(Object^ sender, EventArgs^ e)
    {
        Button^ btn = safe_cast<Button^>(sender);
        btn->BackColor = Color::FromArgb(80, 0, 0);
    }

    void btnKlondike_Click(Object^ sender, EventArgs^ e)
    {
        this->Hide();
        GameForm^ form = gcnew GameForm();
        form->ShowDialog();
        this->Show();
    }

    void btnSpider_Click(Object^ sender, EventArgs^ e)
    {
        this->Hide();
        SpiderGameForm^ form = gcnew SpiderGameForm(1);
        form->ShowDialog();
        this->Show();
    }

    void btnExit_Click(Object^ sender, EventArgs^ e)
    {
        Application::Exit();
    }
};