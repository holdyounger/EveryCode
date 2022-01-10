/********************************************************************************
 *
 *        Copyright (C) 2021  Beijing winicssec Technology
 *        All rights reserved
 *
 *        filename		: myString.h
 *        description	:    
 *
 *        created by  mingming.shi on Jul. 14th, 2021
 *
 ********************************************************************************/
#pragma warning(disable: 4996)
#include <iostream>
#include <vector>
#include <string>
using namespace std;

class MyString
{
public:
	MyString(const char *s);
	MyString(const MyString &s);  //拷贝构造函数
	MyString(void); //不带参数的构造函数
	~MyString(void);              //析构函数
	MyString& operator=(const MyString &s); //=号运算符重载
	MyString operator+(const MyString &s);//+号运算符重载

	char operator[](int i);//[]运算符重载
	bool operator<(const MyString &s);//<运算符重载
	bool operator>(const MyString &s);//>运算符重载
	bool operator==(const MyString &s);//==运算符重载
	bool operator!=(const MyString &s);//!=运算符重载

	MyString ToUpper(void);
	MyString ToLower(void);
	MyString GetStr(const int& n,const size_t& len);

	int FindStr(const char* str);
	int Length(void);
	int Compare(const MyString &s);
	MyString RemoveLRSpace(void);
private:
	char *m_str;
	//友元函数
	friend ostream& operator<<(ostream &out, MyString &s);//重载左移运算符
	friend istream& operator >> (istream &in, MyString &s); //重载右移运算符
};