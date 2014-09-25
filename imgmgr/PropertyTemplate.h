#pragma once
// templatized implementation for class property syntactic sugar

#include <iostream>
#include <string>

using namespace std;

// ------------------------------------------------------------------

#define PROPERTY_GET_SET(CLASS, NAME, TYPE) GetSetProperty<CLASS, TYPE> NAME() { return GetSetProperty<CLASS, TYPE>(this, &CLASS::get_##NAME, &CLASS::set_##NAME); }
#define PROPERTY_GET(CLASS, NAME, TYPE)     GetProperty<CLASS, TYPE> NAME()    { return GetProperty<CLASS, TYPE>(this, &CLASS::get_##NAME); }
#define PROPERTY_SET(CLASS, NAME, TYPE)     SetProperty<CLASS, TYPE> NAME()    { return SetProperty<CLASS, TYPE>(this, &CLASS::set_##NAME); }

template <typename CLASS, typename TYPE>
struct GetSetProperty {
	typedef TYPE(CLASS::*Getter_t)() const;
	typedef void (CLASS::*Setter_t)(TYPE);
	GetSetProperty(CLASS* instance, Getter_t getter, Setter_t setter) : m_instance(instance), m_getter(getter), m_setter(setter) {}
	operator TYPE() const { return (this->m_instance->*this->m_getter)(); }
	GetSetProperty<CLASS, TYPE>& operator=(TYPE value) { (this->m_instance->*this->m_setter)(value); return *this; }
	CLASS* const   m_instance;
	const Getter_t m_getter;
	const Setter_t m_setter;
};

template <typename CLASS, typename TYPE>
struct GetProperty {
	typedef TYPE(CLASS::*Getter_t)() const;
	GetProperty(CLASS* instance, Getter_t getter) : m_instance(instance), m_getter(getter) {}
	operator TYPE() const { return (this->m_instance->*this->m_getter)(); }
	CLASS* const   m_instance;
	const Getter_t m_getter;
};

template <typename CLASS, typename TYPE>
struct SetProperty {
	typedef void (CLASS::*Setter_t)(TYPE);
	SetProperty(CLASS* instance, Setter_t setter) : m_instance(instance), m_setter(setter) {}
	SetProperty<CLASS, TYPE>& operator=(TYPE value) { (this->m_instance->*this->m_setter)(value); return *this; }
	CLASS* const   m_instance;
	const Setter_t m_setter;
};

template <typename CLASS, typename TYPE>
ostream& operator<<(ostream& ostr, const GetSetProperty<CLASS, TYPE>& p) { ostr << (p.m_instance->*p.m_getter)(); return ostr; }

template <typename CLASS, typename TYPE>
ostream& operator<<(ostream& ostr, const GetProperty<CLASS, TYPE>& p) { ostr << (p.m_instance->*p.m_getter)(); return ostr; }

//template <typename CLASS, typename TYPE>
//CLASS& operator() ()
//{
//	return _value;
//}
//
//template <typename CLASS, typename TYPE>
//CLASS const& operator() () const
//{
//	return _value;
//}
//class Dummy
//{
//public:
//
//	Dummy() : m_value1(42) {}
//
//	PROPERTY_GET_SET(Dummy, Value1, int);
//	PROPERTY_GET_SET(Dummy, Value2, const string&);
//
//protected:
//
//	virtual int           get_Value1() const { return this->m_value1; }
//	virtual void          set_Value1(int value) { this->m_value1 = value; }
//
//	virtual const string& get_Value2() const { return this->m_value2; }
//	virtual void          set_Value2(const string& value) { this->m_value2 = value; }
//
//private:
//
//	int    m_value1;
//	string m_value2;
//};
//
//
//int main(int argc, char* argv[]) {
//
//	Dummy d;
//
//	cout << d.Value1() << endl;
//	d.Value1() = 3;
//	cout << d.Value1() << endl;
//
//	cout << d.Value2() << endl;
//	d.Value2() = "test";
//	cout << d.Value2() << endl;
//
//	return 0;
//}


// Alternate (and simpler) approach
//template<typename T>
//class Property
//{
//private:
//	T& _value;
//
//public:
//	Property(T& value) : _value(value)
//	{
//	}   // eo ctor
//
//	Property<T>& operator = (const T& val)
//	{
//		_value = val;
//		return *this;
//	};  // eo operator =
//
//	operator const T&() const
//	{
//		return _value;
//	};  // eo operator ()
//
//	T& operator() ()
//	{
//		return _value;
//	}
//	T const& operator() () const
//	{
//		return _value;
//	}
//};
