////////////////////////////////////////////////////////////////////////
//
// SchemaTypeCalendar.h
//
// This file was generated by XMLSpy 2005 Enterprise Edition.
//
// YOU SHOULD NOT MODIFY THIS FILE, BECAUSE IT WILL BE
// OVERWRITTEN WHEN YOU RE-RUN CODE GENERATION.
//
// Refer to the XMLSpy Documentation for further details.
// http://www.altova.com/xmlspy
//
////////////////////////////////////////////////////////////////////////


#include "Altova.h"
#include "AltovaException.h"
#include <math.h>
#include <time.h>

#include "SchemaTypeCalendar.h"



namespace altova {


////////////////////////////////////////////////////////////////////////
//
//  CSchemaTypeCalendar
//
////////////////////////////////////////////////////////////////////////

bool	CSchemaTypeCalendar::IsConvertableTo( CSchemaType::ETypes eType ) const
{
	try
	{
		switch( eType )
		{
		case CSchemaType::k_Duration:	return &operator CSchemaTypeCalendar::CDuration() != NULL;
		case CSchemaType::k_Time:		return &operator CSchemaTypeCalendar::CTime() != NULL;
		case CSchemaType::k_Day:		return &operator CSchemaTypeCalendar::CDay() != NULL;
		case CSchemaType::k_Month:		return &operator CSchemaTypeCalendar::CMonth() != NULL;
		case CSchemaType::k_MonthDay:	return &operator CSchemaTypeCalendar::CMonthDay() != NULL;
		case CSchemaType::k_Year:		return &operator CSchemaTypeCalendar::CYear() != NULL;
		case CSchemaType::k_YearMonth:	return &operator CSchemaTypeCalendar::CYearMonth() != NULL;
		case CSchemaType::k_Date:		return &operator CSchemaTypeCalendar::CDate() != NULL;
		case CSchemaType::k_DateTime:	return &operator CSchemaTypeCalendar::CDateTime() != NULL;
		}
	}
	catch( CAltovaException& )
	{}
	return false;
}


//  Utility functions
////////////////////////////////////////////////////////////////////////

int CSchemaTypeCalendar::ParseInt(const TCHAR** pszData, const TCHAR* szEstimatedType, const TCHAR* szValue, const int nMin, const int nMax )
{
	TCHAR* szEndPtr;
	int n = _tcstol(*pszData, &szEndPtr, 10);
	if (*pszData == szEndPtr)
		ThrowFormatError();
	*pszData = szEndPtr;
	if ( ( nMin != INT_MIN  &&  n < nMin ) || ( nMax != INT_MAX  &&  n > nMax ) )
		ThrowOutOfRangeError();
	return n;
}


double CSchemaTypeCalendar::ParseDouble(const TCHAR** pszData, const TCHAR* szEstimatedType, const TCHAR* szValue)
{
	TCHAR* szEndPtr;
	double d = _tcstod(*pszData, &szEndPtr);
	if (*pszData == szEndPtr)
		ThrowFormatError();
	*pszData = szEndPtr;
	return d;
}


void CSchemaTypeCalendar::ParseChar(const TCHAR** pszData, TCHAR chValue, const TCHAR* szEstimatedType, const TCHAR* szValue)
{
	if (**pszData != chValue)
		ThrowFormatError();
	(*pszData)++;
}


inline double CSchemaTypeCalendar::fQuotient(double a, double b)
{
	return floor(a / b);
}


inline double CSchemaTypeCalendar::fQuotient(double a, double low, double high)
{
	return fQuotient(a - low, high - low);
}


inline double CSchemaTypeCalendar::modulo(double a, double b)
{
	return a - fQuotient(a, b) * b;
}


inline double CSchemaTypeCalendar::modulo(double a, double low, double high)
{
	return modulo(a - low, high - low) + low;
}


inline bool CSchemaTypeCalendar::isLeapYear(int year)
{
	return (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
}


int CSchemaTypeCalendar::maxDayInMonthFor(int year, int month)
{
	if (month == 4 || month == 6 || month == 9 || month == 11)
		return 30;
	else if (month == 2)
		return isLeapYear(year) ? 29 : 28;
	else
		return 31;
}


void CSchemaTypeCalendar::DateTimeAddDuration( int& nYear, int& nMonth, int& nDay, int& nHour, int& nMinute, double& dSecond, const CSchemaTypeCalendar::CDuration& dur )
{
	double temp, carry;

	temp = nMonth + dur.nMonth;
	nMonth = (int)modulo(temp, 1, 13);
	carry = fQuotient(temp, 1, 13);

	nYear += dur.nYear + (int)carry;

	temp = dSecond + dur.dSecond;
	dSecond = modulo(temp, 60);
	carry = fQuotient(temp, 60);

	temp = nMinute + dur.nMinute + carry;
	nMinute = (int)modulo(temp, 60);
	carry = fQuotient(temp, 60);

	temp = nHour + dur.nHour + carry;
	nHour = (int)modulo(temp, 24);
	carry = fQuotient(temp, 24);

	temp = maxDayInMonthFor(nYear, nMonth);
	if (nDay <= temp)
		temp = nDay < 1 ? 1 : nDay;
	nDay = (int)(temp + dur.nDay + carry);

	while (true)
	{
		temp = maxDayInMonthFor(nYear, nMonth);
		if (nDay < 1)
		{
			nDay += maxDayInMonthFor(nYear, nMonth - 1);
			carry = -1;
		}
		else if (nDay > temp)
		{
			nDay -= (int)temp;
			carry = 1;
		}
		else
		{
			break;
		}

		temp = nMonth + carry;
		nMonth = (int)modulo(temp, 1, 13);
		nYear += (int)fQuotient(temp, 1, 13);
	}
}


int CSchemaTypeCalendar::WriteTime( TCHAR* szTarget, int nHour, int nMinute, double dSecond )
{
	if (dSecond - floor(dSecond) == 0)
	{
		return _stprintf(szTarget, _T("%02i:%02i:%02.f"), nHour, nMinute, floor(dSecond));
	}
	else
	{
		TCHAR* szLast = szTarget + _stprintf(szTarget, _T("%02i:%02i:%02.9f"), nHour, nMinute, dSecond);

		while (*--szLast == _T('0'))
			*szLast = 0;

		return szLast - szTarget;
	}
}


void CSchemaTypeCalendar::ParseTZ( CDateTimeBase& rTimezone, const TCHAR* szValue )
{
	const TCHAR* szTmp = szValue;

	if (*szTmp == 0)
	{
		rTimezone.nTZMode = CSchemaTypeCalendar::CDateTimeBase::TZ_Missing;
		rTimezone.nOffset = 0;
	}
	else if (*szTmp == _T('Z'))
	{
		rTimezone.nTZMode = CSchemaTypeCalendar::CDateTimeBase::TZ_UTC;
		rTimezone.nOffset = 0;
		if (*++szTmp)
			ThrowFormatError();
	}
	else
	{
		rTimezone.nTZMode = CSchemaTypeCalendar::CDateTimeBase::TZ_Offset;
		int nOffsetHour = ParseInt(&szTmp, _T("datetime BASE"), szValue, -24, 24 );
		ParseChar(&szTmp, _T(':'), _T("datetime BASE"), szValue);
		int nOffsetMinute = ParseInt(&szTmp, _T("datetime BASE"), szValue, 0, 59 );
		if (nOffsetHour < 0)
			rTimezone.nOffset = nOffsetHour * 60 - nOffsetMinute;
		else
			rTimezone.nOffset = nOffsetHour * 60 + nOffsetMinute;
		if (*szTmp)
			ThrowFormatError();
	}
}


void CSchemaTypeCalendar::WriteTZ( const CDateTimeBase& rTimezone, TCHAR* szTarget )
{
	switch (rTimezone.nTZMode)
	{
	case CSchemaTypeCalendar::CDateTimeBase::TZ_UTC:
		_tcscat(szTarget, _T("Z"));
		break;
	
	case CSchemaTypeCalendar::CDateTimeBase::TZ_Offset:
		_stprintf( szTarget + _tcslen(szTarget), _T("%c%02i:%02i"),
				rTimezone.nOffset < 0 ? _T('-') : _T('+'),
				abs( rTimezone.nOffset / 60 ),
				abs( rTimezone.nOffset % 60 )
				);
		break;
	}
}


////////////////////////////////////////////////////////////////////////
//
//  Operators
//
////////////////////////////////////////////////////////////////////////

// CSchemaDuration
CSchemaDuration ALTOVA_DECLSPECIFIER operator+(const CSchemaDuration& dur1, const CSchemaDuration& dur2)
{
	CSchemaTypeCalendar::CDuration dur;

	dur.bPositive	= !( dur1.GetValue().bPositive ^ dur2.GetValue().bPositive );
	dur.nYear		= dur1.GetValue().nYear		+ dur2.GetValue().nYear;
	dur.nMonth		= dur1.GetValue().nMonth	+ dur2.GetValue().nMonth;
	dur.nDay		= dur1.GetValue().nDay		+ dur2.GetValue().nDay;
	dur.nHour		= dur1.GetValue().nHour		+ dur2.GetValue().nHour;
	dur.nMinute		= dur1.GetValue().nMinute	+ dur2.GetValue().nMinute;
	dur.dSecond		= dur1.GetValue().dSecond	+ dur2.GetValue().dSecond;
	return dur;
}


// CSchemaDate

bool ALTOVA_DECLSPECIFIER operator==(const CSchemaDate& t1, const CSchemaDate& t2)
{
	if (t1.GetValue().HasTimezone() == t2.GetValue().HasTimezone())
		return t1.GetValue().nYear == t2.GetValue().nYear
			&& t1.GetValue().nMonth == t2.GetValue().nMonth
			&& t1.GetValue().nDay == t2.GetValue().nDay;
	else
		return false;
}


bool ALTOVA_DECLSPECIFIER operator<(const CSchemaDate& t1, const CSchemaDate& t2)
{
	if (t1.GetValue().HasTimezone() == t2.GetValue().HasTimezone())
		return t1.GetValue().nYear < t2.GetValue().nYear
			|| t1.GetValue().nYear == t2.GetValue().nYear && t1.GetValue().nMonth < t2.GetValue().nMonth
			|| t1.GetValue().nYear == t2.GetValue().nYear && t1.GetValue().nMonth == t2.GetValue().nMonth && t1.GetValue().nDay < t2.GetValue().nDay;
	else
		return false;
}


bool ALTOVA_DECLSPECIFIER operator!=(const CSchemaDate& t1, const CSchemaDate& t2)
{
	return !(t1 == t2);
}


bool ALTOVA_DECLSPECIFIER operator<=(const CSchemaDate& t1, const CSchemaDate& t2)
{
	return t1 < t2 || t1 == t2;
}


bool ALTOVA_DECLSPECIFIER operator>(const CSchemaDate& t1, const CSchemaDate& t2)
{
	return t2 < t1;
}


bool ALTOVA_DECLSPECIFIER operator>=(const CSchemaDate& t1, const CSchemaDate& t2)
{
	return t2 < t1 || t1 == t2;
}


// CSchemaTime

bool ALTOVA_DECLSPECIFIER operator==(const CSchemaTime& t1, const CSchemaTime& t2)
{
	if (t1.GetValue().HasTimezone() == t2.GetValue().HasTimezone())
		return t1.GetValue().NormalizedSeconds() == t2.GetValue().NormalizedSeconds();
	else
		return false;
}


bool ALTOVA_DECLSPECIFIER operator<(const CSchemaTime& t1, const CSchemaTime& t2)
{
	if (t1.GetValue().HasTimezone() == t2.GetValue().HasTimezone())
		return t1.GetValue().NormalizedSeconds() < t2.GetValue().NormalizedSeconds();

	if (t1.GetValue().HasTimezone())
		return t1.GetValue().NormalizedSeconds() < t2.GetValue().NormalizedSeconds() - 50400;
	else
		return t1.GetValue().NormalizedSeconds() + 50400 < t2.GetValue().NormalizedSeconds();
}


bool ALTOVA_DECLSPECIFIER operator!=(const CSchemaTime& t1, const CSchemaTime& t2)
{
	return !(t1 == t2);
}


bool ALTOVA_DECLSPECIFIER operator<=(const CSchemaTime& t1, const CSchemaTime& t2)
{
	return t1 < t2 || t1 == t2;
}


bool ALTOVA_DECLSPECIFIER operator>(const CSchemaTime& t1, const CSchemaTime& t2)
{
	return t2 < t1;
}


bool ALTOVA_DECLSPECIFIER operator>=(const CSchemaTime& t1, const CSchemaTime& t2)
{
	return t2 < t1 || t1 == t2;
}


// CSchemaDateTime

bool ALTOVA_DECLSPECIFIER operator==(const CSchemaDateTime& t1, const CSchemaDateTime& t2)
{
	if (t1.GetValue().HasTimezone() == t2.GetValue().HasTimezone())
		return t1.GetValue().nYear == t2.GetValue().nYear
			&& t1.GetValue().nMonth == t2.GetValue().nMonth
			&& t1.GetValue().nDay == t2.GetValue().nDay
			&& t1.GetValue().nHour == t2.GetValue().nHour
			&& t1.GetValue().nMinute == t2.GetValue().nMinute
			&& t1.GetValue().dSecond == t2.GetValue().dSecond;
	else
		return false;
}


bool ALTOVA_DECLSPECIFIER operator<(const CSchemaDateTime& t1, const CSchemaDateTime& t2)
{
	CSchemaTypeCalendar::CDateTime x1 = t1.GetValue();
	CSchemaTypeCalendar::CDateTime x2 = t2.GetValue();

	if( x1.HasTimezone() != x2.HasTimezone() )
	{
		if( x1.HasTimezone() )
		{
			CSchemaTypeCalendar::CDuration dur;
			dur.bPositive = false;
			dur.nHour = -14;
			CSchemaTypeCalendar::DateTimeAddDuration( x2.nYear, x2.nMonth, x2.nDay, x2.nHour, x2.nMinute, x2.dSecond, dur );
		}
		else
		{
			CSchemaTypeCalendar::CDuration dur;
			dur.bPositive = true;
			dur.nHour = 14;
			CSchemaTypeCalendar::DateTimeAddDuration( x1.nYear, x2.nMonth, x2.nDay, x2.nHour, x2.nMinute, x2.dSecond, dur );
		}
	}

	if (x1.nYear	< x2.nYear)		return true;
	if (x1.nYear	> x2.nYear)		return false;
	if (x1.nMonth	< x2.nMonth)	return true;
	if (x1.nMonth	> x2.nMonth)	return false;
	if (x1.nDay		< x2.nDay)		return true;
	if (x1.nDay		> x2.nDay)		return false;
	if (x1.nHour	< x2.nHour)		return true;
	if (x1.nHour	> x2.nHour)		return false;
	if (x1.nMinute	< x2.nMinute)	return true;
	if (x1.nMinute	> x2.nMinute)	return false;
	if (x1.dSecond	< x2.dSecond)	return true;
	return false;
}


bool ALTOVA_DECLSPECIFIER operator!=(const CSchemaDateTime& t1, const CSchemaDateTime& t2)
{
	return !(t1 == t2);
}


bool ALTOVA_DECLSPECIFIER operator<=(const CSchemaDateTime& t1, const CSchemaDateTime& t2)
{
	return t1 < t2 || t1 == t2;
}


bool ALTOVA_DECLSPECIFIER operator>(const CSchemaDateTime& t1, const CSchemaDateTime& t2)
{
	return t2 < t1;
}


bool ALTOVA_DECLSPECIFIER operator>=(const CSchemaDateTime& t1, const CSchemaDateTime& t2)
{
	return t2 < t1 || t1 == t2;
}


// mixed

CSchemaDate ALTOVA_DECLSPECIFIER operator+(const CSchemaDate& t, const CSchemaDuration& dur)
{
	CSchemaTypeCalendar::CDate	r = t.GetValue();
	int nHour		= 0;
	int nMinute		= 0;
	double dSecond	= 0;

	CSchemaTypeCalendar::DateTimeAddDuration(r.nYear, r.nMonth, r.nDay, nHour, nMinute, dSecond, dur);

	return r;
}

CSchemaDuration ALTOVA_DECLSPECIFIER operator-(const CSchemaDate& t1, const CSchemaDate& t2)
{
	CSchemaTypeCalendar::CDuration	dur;
	dur.bPositive = false;
	dur.nYear = -t2.GetValue().nYear;
	dur.nMonth = -t2.GetValue().nMonth;
	dur.nDay = -t2.GetValue().nDay;

	CSchemaDate t = t1 + CSchemaDuration( dur );
	dur.bPositive = true;
	dur.nYear = t.GetValue().nYear;
	dur.nMonth = t.GetValue().nMonth;
	dur.nDay = t.GetValue().nDay;

	return dur;
}

CSchemaTime ALTOVA_DECLSPECIFIER operator+(const CSchemaTime& t, const CSchemaDuration& dur)
{
	CSchemaTypeCalendar::CTime r	= t.GetValue();
	int nYear		= 1;
	int nMonth		= 1;
	int nDay		= 1;

	CSchemaTypeCalendar::DateTimeAddDuration( nYear, nMonth, nDay, r.nHour, r.nMinute, r.dSecond, dur);

	return r;
}


CSchemaDuration ALTOVA_DECLSPECIFIER operator-(const CSchemaTime& t1, const CSchemaTime& t2)
{
	CSchemaTypeCalendar::CDuration dur;
	dur.bPositive = false;
	dur.nHour = -t2.GetValue().nHour;
	dur.nMinute = -t2.GetValue().nMinute;
	dur.dSecond = -t2.GetValue().dSecond;

	CSchemaTime t = t1 + CSchemaDuration( dur );
	dur.bPositive = true;
	dur.nYear = 0;
	dur.nMonth = 0;
	dur.nDay = 0;
	dur.nHour = t.GetValue().nHour;
	dur.nMinute = t.GetValue().nMinute; 
	dur.dSecond = t.GetValue().dSecond;

	return dur;
}


CSchemaDateTime ALTOVA_DECLSPECIFIER operator+(const CSchemaDateTime& t, const CSchemaDuration& dur)
{
	CSchemaTypeCalendar::CDateTime r = t.GetValue();

	CSchemaTypeCalendar::DateTimeAddDuration( r.nYear, r.nMonth, r.nDay, r.nHour, r.nMinute, r.dSecond, dur );

	return r;
}


CSchemaDuration ALTOVA_DECLSPECIFIER operator-(const CSchemaDateTime& t1, const CSchemaDateTime& t2)
{
	CSchemaTypeCalendar::CDuration dur;
	dur.bPositive = (t1 >= t2);
	dur.nYear = t1.GetValue().nYear - t2.GetValue().nYear;
	dur.nMonth = t1.GetValue().nMonth - t2.GetValue().nMonth;
	dur.nDay = t1.GetValue().nDay - t2.GetValue().nDay;
	dur.nHour = t1.GetValue().nHour - t2.GetValue().nHour;
	dur.nMinute = t1.GetValue().nMinute - t2.GetValue().nMinute;
	dur.dSecond = t1.GetValue().dSecond - t2.GetValue().dSecond;
  
	return dur;
}


} // namespace altova
