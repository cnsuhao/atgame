#pragma once

#include "atgMath.h"

namespace MatrixCalculator {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Form1 摘要
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: 在此处添加构造函数代码
			//
		}

	protected:
		/// <summary>
		/// 清理所有正在使用的资源。
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
    private: System::Windows::Forms::MaskedTextBox^  textBox1;
    private: System::Windows::Forms::MaskedTextBox^  textBox2;
    private: System::Windows::Forms::MaskedTextBox^  textBox3;
    private: System::Windows::Forms::MaskedTextBox^  textBox4;
    private: System::Windows::Forms::MaskedTextBox^  textBox5;
    private: System::Windows::Forms::MaskedTextBox^  textBox6;
    private: System::Windows::Forms::MaskedTextBox^  textBox7;
    private: System::Windows::Forms::MaskedTextBox^  textBox8;
    private: System::Windows::Forms::MaskedTextBox^  textBox9;
    private: System::Windows::Forms::MaskedTextBox^  textBox10;
    private: System::Windows::Forms::MaskedTextBox^  textBox11;
    private: System::Windows::Forms::MaskedTextBox^  textBox12;
    private: System::Windows::Forms::MaskedTextBox^  textBox13;
    private: System::Windows::Forms::MaskedTextBox^  textBox14;
    private: System::Windows::Forms::MaskedTextBox^  textBox15;
    private: System::Windows::Forms::MaskedTextBox^  textBox16;
    private: System::Windows::Forms::MaskedTextBox^  textBox17;
    private: System::Windows::Forms::MaskedTextBox^  textBox18;
    private: System::Windows::Forms::MaskedTextBox^  textBox19;
    private: System::Windows::Forms::MaskedTextBox^  textBox20;
    private: System::Windows::Forms::MaskedTextBox^  textBox21;
    private: System::Windows::Forms::MaskedTextBox^  textBox22;
    private: System::Windows::Forms::MaskedTextBox^  textBox23;
    private: System::Windows::Forms::MaskedTextBox^  textBox24;
    private: System::Windows::Forms::MaskedTextBox^  textBox25;
    private: System::Windows::Forms::MaskedTextBox^  textBox26;
    private: System::Windows::Forms::MaskedTextBox^  textBox27;
    private: System::Windows::Forms::MaskedTextBox^  textBox28;
    private: System::Windows::Forms::MaskedTextBox^  textBox29;
    private: System::Windows::Forms::MaskedTextBox^  textBox30;
    private: System::Windows::Forms::MaskedTextBox^  textBox31;
    private: System::Windows::Forms::MaskedTextBox^  textBox32;
    private: System::Windows::Forms::MaskedTextBox^  textBox33;
    private: System::Windows::Forms::MaskedTextBox^  textBox34;
    private: System::Windows::Forms::MaskedTextBox^  textBox35;
    private: System::Windows::Forms::MaskedTextBox^  textBox36;
    private: System::Windows::Forms::MaskedTextBox^  textBox37;
    private: System::Windows::Forms::MaskedTextBox^  textBox38;
    private: System::Windows::Forms::MaskedTextBox^  textBox39;
    private: System::Windows::Forms::MaskedTextBox^  textBox40;
    private: System::Windows::Forms::MaskedTextBox^  textBox41;
    private: System::Windows::Forms::MaskedTextBox^  textBox42;
    private: System::Windows::Forms::MaskedTextBox^  textBox43;
    private: System::Windows::Forms::MaskedTextBox^  textBox44;
    private: System::Windows::Forms::MaskedTextBox^  textBox45;
    private: System::Windows::Forms::MaskedTextBox^  textBox46;
    private: System::Windows::Forms::MaskedTextBox^  textBox47;
    private: System::Windows::Forms::MaskedTextBox^  textBox48;
    private: System::Windows::Forms::Label^  label1;
    private: System::Windows::Forms::Label^  label2;
    private: System::Windows::Forms::Button^  button1;
    private: System::Windows::Forms::GroupBox^  groupBox1;
    
    private: System::Collections::ArrayList^ textBoxArray_M1;
    private: System::Collections::ArrayList^ textBoxArray_M2;
    private: System::Collections::ArrayList^ textBoxArray_M3;
    private: Matrix* pM1;
    private: Matrix* pM2;
    private: Matrix* pM3;


	private:
		/// <summary>
		/// 必需的设计器变量。
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 设计器支持所需的方法 - 不要
		/// 使用代码编辑器修改此方法的内容。
		/// </summary>
		void InitializeComponent(void)
		{
            this->textBox1 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox2 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox3 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox4 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox5 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox6 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox7 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox8 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox9 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox10 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox11 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox12 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox13 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox14 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox15 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox16 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox17 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox18 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox19 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox20 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox21 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox22 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox23 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox24 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox25 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox26 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox27 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox28 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox29 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox30 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox31 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox32 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox33 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox34 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox35 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox36 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox37 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox38 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox39 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox40 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox41 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox42 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox43 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox44 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox45 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox46 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox47 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->textBox48 = (gcnew System::Windows::Forms::MaskedTextBox());
            this->label1 = (gcnew System::Windows::Forms::Label());
            this->label2 = (gcnew System::Windows::Forms::Label());
            this->button1 = (gcnew System::Windows::Forms::Button());
            this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
            this->SuspendLayout();
            // 
            // textBox1
            // 
            this->textBox1->Location = System::Drawing::Point(14, 14);
            this->textBox1->Mask = L"99999";
            this->textBox1->Name = L"textBox1";
            this->textBox1->Size = System::Drawing::Size(66, 23);
            this->textBox1->TabIndex = 0;
            this->textBox1->ValidatingType = System::Int32::typeid;
            this->textBox1->TextChanged += gcnew System::EventHandler(this, &Form1::M1_TextBox_TextChanged);
            this->textBox1->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M1_TextBox_KeyDown);
            // 
            // textBox2
            // 
            this->textBox2->Location = System::Drawing::Point(87, 14);
            this->textBox2->Name = L"textBox2";
            this->textBox2->Size = System::Drawing::Size(66, 23);
            this->textBox2->TabIndex = 1;
            this->textBox2->TextChanged += gcnew System::EventHandler(this, &Form1::M1_TextBox_TextChanged);
            this->textBox2->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M1_TextBox_KeyDown);
            // 
            // textBox3
            // 
            this->textBox3->Location = System::Drawing::Point(161, 14);
            this->textBox3->Name = L"textBox3";
            this->textBox3->Size = System::Drawing::Size(66, 23);
            this->textBox3->TabIndex = 2;
            this->textBox3->TextChanged += gcnew System::EventHandler(this, &Form1::M1_TextBox_TextChanged);
            this->textBox3->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M1_TextBox_KeyDown);
            // 
            // textBox4
            // 
            this->textBox4->BackColor = System::Drawing::SystemColors::Window;
            this->textBox4->Location = System::Drawing::Point(234, 14);
            this->textBox4->Name = L"textBox4";
            this->textBox4->Size = System::Drawing::Size(66, 23);
            this->textBox4->TabIndex = 3;
            this->textBox4->TextChanged += gcnew System::EventHandler(this, &Form1::M1_TextBox_TextChanged);
            this->textBox4->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M1_TextBox_KeyDown);
            // 
            // textBox5
            // 
            this->textBox5->Location = System::Drawing::Point(14, 45);
            this->textBox5->Name = L"textBox5";
            this->textBox5->Size = System::Drawing::Size(66, 23);
            this->textBox5->TabIndex = 4;
            this->textBox5->TextChanged += gcnew System::EventHandler(this, &Form1::M1_TextBox_TextChanged);
            this->textBox5->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M1_TextBox_KeyDown);
            // 
            // textBox6
            // 
            this->textBox6->Location = System::Drawing::Point(87, 45);
            this->textBox6->Name = L"textBox6";
            this->textBox6->Size = System::Drawing::Size(66, 23);
            this->textBox6->TabIndex = 5;
            this->textBox6->TextChanged += gcnew System::EventHandler(this, &Form1::M1_TextBox_TextChanged);
            this->textBox6->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M1_TextBox_KeyDown);
            // 
            // textBox7
            // 
            this->textBox7->Location = System::Drawing::Point(161, 45);
            this->textBox7->Name = L"textBox7";
            this->textBox7->Size = System::Drawing::Size(66, 23);
            this->textBox7->TabIndex = 6;
            this->textBox7->TextChanged += gcnew System::EventHandler(this, &Form1::M1_TextBox_TextChanged);
            this->textBox7->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M1_TextBox_KeyDown);
            // 
            // textBox8
            // 
            this->textBox8->Location = System::Drawing::Point(234, 45);
            this->textBox8->Name = L"textBox8";
            this->textBox8->Size = System::Drawing::Size(66, 23);
            this->textBox8->TabIndex = 7;
            this->textBox8->TextChanged += gcnew System::EventHandler(this, &Form1::M1_TextBox_TextChanged);
            this->textBox8->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M1_TextBox_KeyDown);
            // 
            // textBox9
            // 
            this->textBox9->Location = System::Drawing::Point(14, 77);
            this->textBox9->Name = L"textBox9";
            this->textBox9->Size = System::Drawing::Size(66, 23);
            this->textBox9->TabIndex = 8;
            this->textBox9->TextChanged += gcnew System::EventHandler(this, &Form1::M1_TextBox_TextChanged);
            this->textBox9->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M1_TextBox_KeyDown);
            // 
            // textBox10
            // 
            this->textBox10->Location = System::Drawing::Point(87, 77);
            this->textBox10->Name = L"textBox10";
            this->textBox10->Size = System::Drawing::Size(66, 23);
            this->textBox10->TabIndex = 9;
            this->textBox10->TextChanged += gcnew System::EventHandler(this, &Form1::M1_TextBox_TextChanged);
            this->textBox10->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M1_TextBox_KeyDown);
            // 
            // textBox11
            // 
            this->textBox11->Location = System::Drawing::Point(161, 77);
            this->textBox11->Name = L"textBox11";
            this->textBox11->Size = System::Drawing::Size(66, 23);
            this->textBox11->TabIndex = 10;
            this->textBox11->TextChanged += gcnew System::EventHandler(this, &Form1::M1_TextBox_TextChanged);
            this->textBox11->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M1_TextBox_KeyDown);
            // 
            // textBox12
            // 
            this->textBox12->Location = System::Drawing::Point(234, 77);
            this->textBox12->Name = L"textBox12";
            this->textBox12->Size = System::Drawing::Size(66, 23);
            this->textBox12->TabIndex = 11;
            this->textBox12->TextChanged += gcnew System::EventHandler(this, &Form1::M1_TextBox_TextChanged);
            this->textBox12->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M1_TextBox_KeyDown);
            // 
            // textBox13
            // 
            this->textBox13->Location = System::Drawing::Point(14, 108);
            this->textBox13->Name = L"textBox13";
            this->textBox13->Size = System::Drawing::Size(66, 23);
            this->textBox13->TabIndex = 12;
            this->textBox13->TextChanged += gcnew System::EventHandler(this, &Form1::M1_TextBox_TextChanged);
            this->textBox13->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M1_TextBox_KeyDown);
            // 
            // textBox14
            // 
            this->textBox14->Location = System::Drawing::Point(87, 108);
            this->textBox14->Name = L"textBox14";
            this->textBox14->Size = System::Drawing::Size(66, 23);
            this->textBox14->TabIndex = 13;
            this->textBox14->TextChanged += gcnew System::EventHandler(this, &Form1::M1_TextBox_TextChanged);
            this->textBox14->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M1_TextBox_KeyDown);
            // 
            // textBox15
            // 
            this->textBox15->Location = System::Drawing::Point(161, 108);
            this->textBox15->Name = L"textBox15";
            this->textBox15->Size = System::Drawing::Size(66, 23);
            this->textBox15->TabIndex = 14;
            this->textBox15->TextChanged += gcnew System::EventHandler(this, &Form1::M1_TextBox_TextChanged);
            this->textBox15->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M1_TextBox_KeyDown);
            // 
            // textBox16
            // 
            this->textBox16->Location = System::Drawing::Point(234, 108);
            this->textBox16->Name = L"textBox16";
            this->textBox16->Size = System::Drawing::Size(66, 23);
            this->textBox16->TabIndex = 15;
            this->textBox16->TextChanged += gcnew System::EventHandler(this, &Form1::M1_TextBox_TextChanged);
            this->textBox16->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M1_TextBox_KeyDown);
            // 
            // textBox17
            // 
            this->textBox17->Location = System::Drawing::Point(337, 14);
            this->textBox17->Name = L"textBox17";
            this->textBox17->Size = System::Drawing::Size(66, 23);
            this->textBox17->TabIndex = 31;
            this->textBox17->TextChanged += gcnew System::EventHandler(this, &Form1::M2_TextBox_TextChanged);
            this->textBox17->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M2_TextBox_KeyDown);
            // 
            // textBox18
            // 
            this->textBox18->Location = System::Drawing::Point(409, 14);
            this->textBox18->Name = L"textBox18";
            this->textBox18->Size = System::Drawing::Size(66, 23);
            this->textBox18->TabIndex = 30;
            this->textBox18->TextChanged += gcnew System::EventHandler(this, &Form1::M2_TextBox_TextChanged);
            this->textBox18->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M2_TextBox_KeyDown);
            // 
            // textBox19
            // 
            this->textBox19->Location = System::Drawing::Point(481, 14);
            this->textBox19->Name = L"textBox19";
            this->textBox19->Size = System::Drawing::Size(66, 23);
            this->textBox19->TabIndex = 29;
            this->textBox19->TextChanged += gcnew System::EventHandler(this, &Form1::M2_TextBox_TextChanged);
            this->textBox19->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M2_TextBox_KeyDown);
            // 
            // textBox20
            // 
            this->textBox20->Location = System::Drawing::Point(553, 14);
            this->textBox20->Name = L"textBox20";
            this->textBox20->Size = System::Drawing::Size(66, 23);
            this->textBox20->TabIndex = 28;
            this->textBox20->TextChanged += gcnew System::EventHandler(this, &Form1::M2_TextBox_TextChanged);
            this->textBox20->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M2_TextBox_KeyDown);
            // 
            // textBox21
            // 
            this->textBox21->Location = System::Drawing::Point(337, 45);
            this->textBox21->Name = L"textBox21";
            this->textBox21->Size = System::Drawing::Size(66, 23);
            this->textBox21->TabIndex = 27;
            this->textBox21->TextChanged += gcnew System::EventHandler(this, &Form1::M2_TextBox_TextChanged);
            this->textBox21->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M2_TextBox_KeyDown);
            // 
            // textBox22
            // 
            this->textBox22->Location = System::Drawing::Point(409, 45);
            this->textBox22->Name = L"textBox22";
            this->textBox22->Size = System::Drawing::Size(66, 23);
            this->textBox22->TabIndex = 26;
            this->textBox22->TextChanged += gcnew System::EventHandler(this, &Form1::M2_TextBox_TextChanged);
            this->textBox22->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M2_TextBox_KeyDown);
            // 
            // textBox23
            // 
            this->textBox23->Location = System::Drawing::Point(481, 45);
            this->textBox23->Name = L"textBox23";
            this->textBox23->Size = System::Drawing::Size(66, 23);
            this->textBox23->TabIndex = 25;
            this->textBox23->TextChanged += gcnew System::EventHandler(this, &Form1::M2_TextBox_TextChanged);
            this->textBox23->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M2_TextBox_KeyDown);
            // 
            // textBox24
            // 
            this->textBox24->Location = System::Drawing::Point(553, 45);
            this->textBox24->Name = L"textBox24";
            this->textBox24->Size = System::Drawing::Size(66, 23);
            this->textBox24->TabIndex = 24;
            this->textBox24->TextChanged += gcnew System::EventHandler(this, &Form1::M2_TextBox_TextChanged);
            this->textBox24->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M2_TextBox_KeyDown);
            // 
            // textBox25
            // 
            this->textBox25->Location = System::Drawing::Point(337, 77);
            this->textBox25->Name = L"textBox25";
            this->textBox25->Size = System::Drawing::Size(66, 23);
            this->textBox25->TabIndex = 23;
            this->textBox25->TextChanged += gcnew System::EventHandler(this, &Form1::M2_TextBox_TextChanged);
            this->textBox25->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M2_TextBox_KeyDown);
            // 
            // textBox26
            // 
            this->textBox26->Location = System::Drawing::Point(409, 77);
            this->textBox26->Name = L"textBox26";
            this->textBox26->Size = System::Drawing::Size(66, 23);
            this->textBox26->TabIndex = 22;
            this->textBox26->TextChanged += gcnew System::EventHandler(this, &Form1::M2_TextBox_TextChanged);
            this->textBox26->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M2_TextBox_KeyDown);
            // 
            // textBox27
            // 
            this->textBox27->Location = System::Drawing::Point(481, 77);
            this->textBox27->Name = L"textBox27";
            this->textBox27->Size = System::Drawing::Size(66, 23);
            this->textBox27->TabIndex = 21;
            this->textBox27->TextChanged += gcnew System::EventHandler(this, &Form1::M2_TextBox_TextChanged);
            this->textBox27->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M2_TextBox_KeyDown);
            // 
            // textBox28
            // 
            this->textBox28->Location = System::Drawing::Point(553, 77);
            this->textBox28->Name = L"textBox28";
            this->textBox28->Size = System::Drawing::Size(66, 23);
            this->textBox28->TabIndex = 20;
            this->textBox28->TextChanged += gcnew System::EventHandler(this, &Form1::M2_TextBox_TextChanged);
            this->textBox28->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M2_TextBox_KeyDown);
            // 
            // textBox29
            // 
            this->textBox29->Location = System::Drawing::Point(337, 108);
            this->textBox29->Name = L"textBox29";
            this->textBox29->Size = System::Drawing::Size(66, 23);
            this->textBox29->TabIndex = 19;
            this->textBox29->TextChanged += gcnew System::EventHandler(this, &Form1::M2_TextBox_TextChanged);
            this->textBox29->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M2_TextBox_KeyDown);
            // 
            // textBox30
            // 
            this->textBox30->Location = System::Drawing::Point(409, 108);
            this->textBox30->Name = L"textBox30";
            this->textBox30->Size = System::Drawing::Size(66, 23);
            this->textBox30->TabIndex = 18;
            this->textBox30->TextChanged += gcnew System::EventHandler(this, &Form1::M2_TextBox_TextChanged);
            this->textBox30->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M2_TextBox_KeyDown);
            // 
            // textBox31
            // 
            this->textBox31->Location = System::Drawing::Point(481, 108);
            this->textBox31->Name = L"textBox31";
            this->textBox31->Size = System::Drawing::Size(66, 23);
            this->textBox31->TabIndex = 17;
            this->textBox31->TextChanged += gcnew System::EventHandler(this, &Form1::M2_TextBox_TextChanged);
            this->textBox31->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M2_TextBox_KeyDown);
            // 
            // textBox32
            // 
            this->textBox32->Location = System::Drawing::Point(553, 108);
            this->textBox32->Name = L"textBox32";
            this->textBox32->Size = System::Drawing::Size(66, 23);
            this->textBox32->TabIndex = 16;
            this->textBox32->TextChanged += gcnew System::EventHandler(this, &Form1::M2_TextBox_TextChanged);
            this->textBox32->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M2_TextBox_KeyDown);
            // 
            // textBox33
            // 
            this->textBox33->Location = System::Drawing::Point(658, 14);
            this->textBox33->Name = L"textBox33";
            this->textBox33->Size = System::Drawing::Size(66, 23);
            this->textBox33->TabIndex = 47;
            this->textBox33->TextChanged += gcnew System::EventHandler(this, &Form1::M3_TextBox_TextChanged);
            this->textBox33->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M3_TextBox_KeyDown);
            // 
            // textBox34
            // 
            this->textBox34->Location = System::Drawing::Point(731, 14);
            this->textBox34->Name = L"textBox34";
            this->textBox34->Size = System::Drawing::Size(66, 23);
            this->textBox34->TabIndex = 46;
            this->textBox34->TextChanged += gcnew System::EventHandler(this, &Form1::M3_TextBox_TextChanged);
            this->textBox34->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M3_TextBox_KeyDown);
            // 
            // textBox35
            // 
            this->textBox35->Location = System::Drawing::Point(805, 14);
            this->textBox35->Name = L"textBox35";
            this->textBox35->Size = System::Drawing::Size(66, 23);
            this->textBox35->TabIndex = 45;
            this->textBox35->TextChanged += gcnew System::EventHandler(this, &Form1::M3_TextBox_TextChanged);
            this->textBox35->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M3_TextBox_KeyDown);
            // 
            // textBox36
            // 
            this->textBox36->Location = System::Drawing::Point(878, 14);
            this->textBox36->Name = L"textBox36";
            this->textBox36->Size = System::Drawing::Size(66, 23);
            this->textBox36->TabIndex = 44;
            this->textBox36->TextChanged += gcnew System::EventHandler(this, &Form1::M3_TextBox_TextChanged);
            this->textBox36->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M3_TextBox_KeyDown);
            // 
            // textBox37
            // 
            this->textBox37->Location = System::Drawing::Point(658, 45);
            this->textBox37->Name = L"textBox37";
            this->textBox37->Size = System::Drawing::Size(66, 23);
            this->textBox37->TabIndex = 43;
            this->textBox37->TextChanged += gcnew System::EventHandler(this, &Form1::M3_TextBox_TextChanged);
            this->textBox37->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M3_TextBox_KeyDown);
            // 
            // textBox38
            // 
            this->textBox38->Location = System::Drawing::Point(731, 45);
            this->textBox38->Name = L"textBox38";
            this->textBox38->Size = System::Drawing::Size(66, 23);
            this->textBox38->TabIndex = 42;
            this->textBox38->TextChanged += gcnew System::EventHandler(this, &Form1::M3_TextBox_TextChanged);
            this->textBox38->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M3_TextBox_KeyDown);
            // 
            // textBox39
            // 
            this->textBox39->Location = System::Drawing::Point(805, 45);
            this->textBox39->Name = L"textBox39";
            this->textBox39->Size = System::Drawing::Size(66, 23);
            this->textBox39->TabIndex = 41;
            this->textBox39->TextChanged += gcnew System::EventHandler(this, &Form1::M3_TextBox_TextChanged);
            this->textBox39->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M3_TextBox_KeyDown);
            // 
            // textBox40
            // 
            this->textBox40->Location = System::Drawing::Point(878, 45);
            this->textBox40->Name = L"textBox40";
            this->textBox40->Size = System::Drawing::Size(66, 23);
            this->textBox40->TabIndex = 40;
            this->textBox40->TextChanged += gcnew System::EventHandler(this, &Form1::M3_TextBox_TextChanged);
            this->textBox40->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M3_TextBox_KeyDown);
            // 
            // textBox41
            // 
            this->textBox41->Location = System::Drawing::Point(658, 77);
            this->textBox41->Name = L"textBox41";
            this->textBox41->Size = System::Drawing::Size(66, 23);
            this->textBox41->TabIndex = 39;
            this->textBox41->TextChanged += gcnew System::EventHandler(this, &Form1::M3_TextBox_TextChanged);
            this->textBox41->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M3_TextBox_KeyDown);
            // 
            // textBox42
            // 
            this->textBox42->Location = System::Drawing::Point(731, 77);
            this->textBox42->Name = L"textBox42";
            this->textBox42->Size = System::Drawing::Size(66, 23);
            this->textBox42->TabIndex = 38;
            this->textBox42->TextChanged += gcnew System::EventHandler(this, &Form1::M3_TextBox_TextChanged);
            this->textBox42->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M3_TextBox_KeyDown);
            // 
            // textBox43
            // 
            this->textBox43->Location = System::Drawing::Point(805, 77);
            this->textBox43->Name = L"textBox43";
            this->textBox43->Size = System::Drawing::Size(66, 23);
            this->textBox43->TabIndex = 37;
            this->textBox43->TextChanged += gcnew System::EventHandler(this, &Form1::M3_TextBox_TextChanged);
            this->textBox43->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M3_TextBox_KeyDown);
            // 
            // textBox44
            // 
            this->textBox44->Location = System::Drawing::Point(878, 77);
            this->textBox44->Name = L"textBox44";
            this->textBox44->Size = System::Drawing::Size(66, 23);
            this->textBox44->TabIndex = 36;
            this->textBox44->TextChanged += gcnew System::EventHandler(this, &Form1::M3_TextBox_TextChanged);
            this->textBox44->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M3_TextBox_KeyDown);
            // 
            // textBox45
            // 
            this->textBox45->Location = System::Drawing::Point(658, 108);
            this->textBox45->Name = L"textBox45";
            this->textBox45->Size = System::Drawing::Size(66, 23);
            this->textBox45->TabIndex = 35;
            this->textBox45->TextChanged += gcnew System::EventHandler(this, &Form1::M3_TextBox_TextChanged);
            this->textBox45->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M3_TextBox_KeyDown);
            // 
            // textBox46
            // 
            this->textBox46->Location = System::Drawing::Point(731, 108);
            this->textBox46->Name = L"textBox46";
            this->textBox46->Size = System::Drawing::Size(66, 23);
            this->textBox46->TabIndex = 34;
            this->textBox46->TextChanged += gcnew System::EventHandler(this, &Form1::M3_TextBox_TextChanged);
            this->textBox46->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M3_TextBox_KeyDown);
            // 
            // textBox47
            // 
            this->textBox47->Location = System::Drawing::Point(805, 108);
            this->textBox47->Name = L"textBox47";
            this->textBox47->Size = System::Drawing::Size(66, 23);
            this->textBox47->TabIndex = 33;
            this->textBox47->TextChanged += gcnew System::EventHandler(this, &Form1::M3_TextBox_TextChanged);
            this->textBox47->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M3_TextBox_KeyDown);
            // 
            // textBox48
            // 
            this->textBox48->Location = System::Drawing::Point(878, 108);
            this->textBox48->Name = L"textBox48";
            this->textBox48->Size = System::Drawing::Size(66, 23);
            this->textBox48->TabIndex = 32;
            this->textBox48->TextChanged += gcnew System::EventHandler(this, &Form1::M3_TextBox_TextChanged);
            this->textBox48->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::M3_TextBox_KeyDown);
            // 
            // label1
            // 
            this->label1->AutoSize = true;
            this->label1->Location = System::Drawing::Point(311, 68);
            this->label1->Name = L"label1";
            this->label1->Size = System::Drawing::Size(14, 14);
            this->label1->TabIndex = 48;
            this->label1->Text = L"X";
            // 
            // label2
            // 
            this->label2->AutoSize = true;
            this->label2->Location = System::Drawing::Point(632, 68);
            this->label2->Name = L"label2";
            this->label2->Size = System::Drawing::Size(14, 14);
            this->label2->TabIndex = 52;
            this->label2->Text = L"=";
            // 
            // button1
            // 
            this->button1->Location = System::Drawing::Point(757, 138);
            this->button1->Name = L"button1";
            this->button1->Size = System::Drawing::Size(87, 27);
            this->button1->TabIndex = 51;
            this->button1->Text = L"给我计算";
            this->button1->UseVisualStyleBackColor = true;
            this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click);
            // 
            // groupBox1
            // 
            this->groupBox1->Location = System::Drawing::Point(14, 165);
            this->groupBox1->Name = L"groupBox1";
            this->groupBox1->Size = System::Drawing::Size(932, 136);
            this->groupBox1->TabIndex = 50;
            this->groupBox1->TabStop = false;
            this->groupBox1->Text = L"表达式";
            // 
            // Form1
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(7, 14);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(964, 312);
            this->Controls->Add(this->groupBox1);
            this->Controls->Add(this->button1);
            this->Controls->Add(this->label2);
            this->Controls->Add(this->label1);
            this->Controls->Add(this->textBox33);
            this->Controls->Add(this->textBox34);
            this->Controls->Add(this->textBox35);
            this->Controls->Add(this->textBox36);
            this->Controls->Add(this->textBox37);
            this->Controls->Add(this->textBox38);
            this->Controls->Add(this->textBox39);
            this->Controls->Add(this->textBox40);
            this->Controls->Add(this->textBox41);
            this->Controls->Add(this->textBox42);
            this->Controls->Add(this->textBox43);
            this->Controls->Add(this->textBox44);
            this->Controls->Add(this->textBox45);
            this->Controls->Add(this->textBox46);
            this->Controls->Add(this->textBox47);
            this->Controls->Add(this->textBox48);
            this->Controls->Add(this->textBox17);
            this->Controls->Add(this->textBox18);
            this->Controls->Add(this->textBox19);
            this->Controls->Add(this->textBox20);
            this->Controls->Add(this->textBox21);
            this->Controls->Add(this->textBox22);
            this->Controls->Add(this->textBox23);
            this->Controls->Add(this->textBox24);
            this->Controls->Add(this->textBox25);
            this->Controls->Add(this->textBox26);
            this->Controls->Add(this->textBox27);
            this->Controls->Add(this->textBox28);
            this->Controls->Add(this->textBox29);
            this->Controls->Add(this->textBox30);
            this->Controls->Add(this->textBox31);
            this->Controls->Add(this->textBox32);
            this->Controls->Add(this->textBox16);
            this->Controls->Add(this->textBox15);
            this->Controls->Add(this->textBox14);
            this->Controls->Add(this->textBox13);
            this->Controls->Add(this->textBox12);
            this->Controls->Add(this->textBox11);
            this->Controls->Add(this->textBox10);
            this->Controls->Add(this->textBox9);
            this->Controls->Add(this->textBox8);
            this->Controls->Add(this->textBox7);
            this->Controls->Add(this->textBox6);
            this->Controls->Add(this->textBox5);
            this->Controls->Add(this->textBox4);
            this->Controls->Add(this->textBox3);
            this->Controls->Add(this->textBox2);
            this->Controls->Add(this->textBox1);
            this->Font = (gcnew System::Drawing::Font(L"宋体", 10.5F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
                static_cast<System::Byte>(134)));
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
            this->MaximizeBox = false;
            this->MinimizeBox = false;
            this->Name = L"Form1";
            this->Text = L"矩阵计算器";
            this->Shown += gcnew System::EventHandler(this, &Form1::Form1_Shown);
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion
    private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) 
             {

                 ConcatTransforms(pM2->m, pM1->m, pM3->m);

                 SetMatrixControl(textBoxArray_M1, *pM1);
                 SetMatrixControl(textBoxArray_M2, *pM2);
                 SetMatrixControl(textBoxArray_M3, *pM3);
             }

    private: System::Void InitMatrix()
             {
                 textBoxArray_M1 = gcnew System::Collections::ArrayList;
                 textBoxArray_M1->Add(this->textBox1);
                 textBoxArray_M1->Add(this->textBox2);
                 textBoxArray_M1->Add(this->textBox3);
                 textBoxArray_M1->Add(this->textBox4);
                 textBoxArray_M1->Add(this->textBox5);
                 textBoxArray_M1->Add(this->textBox6);
                 textBoxArray_M1->Add(this->textBox7);
                 textBoxArray_M1->Add(this->textBox8);
                 textBoxArray_M1->Add(this->textBox9);
                 textBoxArray_M1->Add(this->textBox10);
                 textBoxArray_M1->Add(this->textBox11);
                 textBoxArray_M1->Add(this->textBox12);
                 textBoxArray_M1->Add(this->textBox13);
                 textBoxArray_M1->Add(this->textBox14);
                 textBoxArray_M1->Add(this->textBox15);
                 textBoxArray_M1->Add(this->textBox16);

                 textBoxArray_M2 = gcnew System::Collections::ArrayList;
                 textBoxArray_M2->Add(this->textBox17);
                 textBoxArray_M2->Add(this->textBox18);
                 textBoxArray_M2->Add(this->textBox19);
                 textBoxArray_M2->Add(this->textBox20);
                 textBoxArray_M2->Add(this->textBox21);
                 textBoxArray_M2->Add(this->textBox22);
                 textBoxArray_M2->Add(this->textBox23);
                 textBoxArray_M2->Add(this->textBox24);
                 textBoxArray_M2->Add(this->textBox25);
                 textBoxArray_M2->Add(this->textBox26);
                 textBoxArray_M2->Add(this->textBox27);
                 textBoxArray_M2->Add(this->textBox28);
                 textBoxArray_M2->Add(this->textBox29);
                 textBoxArray_M2->Add(this->textBox30);
                 textBoxArray_M2->Add(this->textBox31);
                 textBoxArray_M2->Add(this->textBox32);

                 textBoxArray_M3 = gcnew System::Collections::ArrayList;
                 textBoxArray_M3->Add(this->textBox33);
                 textBoxArray_M3->Add(this->textBox34);
                 textBoxArray_M3->Add(this->textBox35);
                 textBoxArray_M3->Add(this->textBox36);
                 textBoxArray_M3->Add(this->textBox37);
                 textBoxArray_M3->Add(this->textBox38);
                 textBoxArray_M3->Add(this->textBox39);
                 textBoxArray_M3->Add(this->textBox40);
                 textBoxArray_M3->Add(this->textBox41);
                 textBoxArray_M3->Add(this->textBox42);
                 textBoxArray_M3->Add(this->textBox43);
                 textBoxArray_M3->Add(this->textBox44);
                 textBoxArray_M3->Add(this->textBox45);
                 textBoxArray_M3->Add(this->textBox46);
                 textBoxArray_M3->Add(this->textBox47);
                 textBoxArray_M3->Add(this->textBox48);

                 pM1 = new Matrix(Matrix_Identity);
                 pM2 = new Matrix(Matrix_Identity);
                 pM3 = new Matrix(Matrix_Identity);

                 SetMatrixControl(textBoxArray_M1, *pM1);
                 SetMatrixControl(textBoxArray_M2, *pM2);
                 SetMatrixControl(textBoxArray_M3, *pM3);
             }

    private: System::Void SetMatrixControl(System::Collections::ArrayList^ list, const Matrix& matrix)
             {
                 for (int i = 0; i < 16; ++i)
                 {
                     System::Windows::Forms::MaskedTextBox^ textBox = (System::Windows::Forms::MaskedTextBox^)list[i];
                     textBox->Text = "" + matrix[i];
                 }
             }
    
    private: System::Void M1_TextBox_TextChanged(System::Object ^sender,System::EventArgs ^e)
             {

             }
    private: System::Void M2_TextBox_TextChanged(System::Object ^sender,System::EventArgs ^e)
             {

             }
    private: System::Void M3_TextBox_TextChanged(System::Object ^sender,System::EventArgs ^e)
             {

             }

    private: System::Void M1_TextBox_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) 
             {
                 if(Char::IsDigit(e->KeyValue))  
                 {  
                    e->Handled = true;  
                 }
                 else
                 {
                    e->Handled = false;
                 }
             }
    private: System::Void M2_TextBox_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) 
             {

             }

    private: System::Void M3_TextBox_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) 
             {

             }

private: System::Void Form1_Shown(System::Object^  sender, System::EventArgs^  e) {
             InitMatrix();
         }

};
}

