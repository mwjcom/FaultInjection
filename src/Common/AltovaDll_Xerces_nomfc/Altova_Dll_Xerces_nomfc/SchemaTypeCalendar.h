////////////////////////////////////////////////////////////////////////
//
// SchemaTypeCalendarr.h
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


#ifndef ALTOVA_SCHEMATYPE_CALENDAR_H_INCLUDED
#define ALTOVA_SCHEMATYPE_CALENDAR_H_INCLUDED

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "SchemaTypes.h"
#include <math.h>
#include <time.h>

namespace altova {


////////////////////////////////////////////////////////////////////////
//
//  CSchemaTypeCalendar
//
////////////////////////////////////////////////////////////////////////

class ALTOVA_DECLSPECIFIER CSchemaTypeCalendar
{
public:
	// ---------- Inner Classes ----------

	struct CDuration
	{
		CDuration(
			bool _bPositive = true,
			int	_nYear = 0,
			int	_nMonth = 0,
			int	_nDay = 0,
			int	_nHour = 0,
			int	_nMinute = 0,
			double	_dSecond = 0.0
		)
			: bPositive( _bPositive )
			, nYear( _nYear )
			, nMonth( _nMonth )
			, nDay( _nDay )
			, nHour( _nHour )
			, nMinute( _nMinute )
			, dSecond( _dSecond )
		{};

		bool		bPositive;
		int		nYear;
		int		nMonth;
		int		nDay;
		int		nHour;
		int		nMinute;
		double		dSecond;

		double	ApproximatedNormalizedSeconds() const
		{
			return ( bPositive ? 1 : -1 ) * ( ((((nYear*12+nMonth)*31+nDay)*24+nHour)*60+nMinute)*60+dSecond );
		}
	};

	struct CDateTimeBase
	{
		enum
		{
			TZ_UTC,
			TZ_Offset,
			TZ_Missing
		};

		CDateTimeBase()
			: nTZMode( TZ_Missing )
			, nOffset( 0 )
		{};

		int	nTZMode;
		int	nOffset;

		bool HasTimezone() const					{ return nTZMode != TZ_Missing; }
		void SetTimezoneUTC( bool bUTC )			{ nTZMode = bUTC ? TZ_UTC : TZ_Missing; nOffset = 0; }
		void SetTimezoneOffset( int offset )		{ nTZMode = TZ_Offset; nOffset = offset; }
		void SetTimezoneMode( int nTZMode )			{ nTZMode = nTZMode; if( nTZMode == TZ_Missing || nTZMode == TZ_UTC) nOffset = 0; }
	};

	struct CDate : public CDateTimeBase
	{
		CDate()
		{
			time_t long_time;
			time( &long_time ); 
			struct tm *newtime = localtime( &long_time ); 

			nYear = newtime->tm_year + 1900;
			nMonth = newtime->tm_mon + 1;
			nDay = newtime->tm_sec;
		};

		CDate(
			int	_nYear,
			int	_nMonth,
			int	_nDay
		)
			: nYear( _nYear )
			, nMonth( _nMonth )
			, nDay( _nDay )
		{};

		int	nYear;
		int	nMonth;
		int	nDay;

		long ApproximatedNormalizedDays() const	{ return (nYear*12+nMonth)*31 + nDay; }
	};

	struct CDateTime : public CDateTimeBase
	{
		CDateTime()
		{
			time_t long_time;
			time( &long_time ); 
			struct tm *newtime = localtime( &long_time ); 
			nYear = newtime->tm_year + 1900;
			nMonth = newtime->tm_mon + 1;
			nDay = newtime->tm_mday;
			nHour = newtime->tm_hour;
			nMinute = newtime->tm_min;
			dSecond = newtime->tm_sec;
		};

		CDateTime(
			int	_nYear,
			int	_nMonth,
			int	_nDay,
			int	_nHour = 0,
			int	_nMinute = 0,
			double	_dSecond = 0
		)
			: nYear( _nYear )
			, nMonth( _nMonth )
			, nDay( _nDay )
			, nHour( _nHour )
			, nMinute( _nMinute )
			, dSecond( _dSecond )
		{};

		CDateTime(
			int	_nYear,
			int	_nMonth,
			int	_nDay,
			int	_nHour,
			int	_nMinute,
			double	_dSecond,
			int _nTimezone 
		)
			: nYear( _nYear )
			, nMonth( _nMonth )
			, nDay( _nDay )
			, nHour( _nHour )
			, nMinute( _nMinute )
			, dSecond( _dSecond )
		{
			SetTimezoneOffset( _nTimezone );
		};

		CDateTime(
			int	_nYear,
			int	_nMonth,
			int	_nDay,
			int	_nHour,
			int	_nMinute,
			double	_dSecond,
			bool _bIsUTC 
		)
			: nYear( _nYear )
			, nMonth( _nMonth )
			, nDay( _nDay )
			, nHour( _nHour )
			, nMinute( _nMinute )
			, dSecond( _dSecond )
		{
			SetTimezoneUTC( _bIsUTC );
		};

		int	nYear;
		int	nMonth;
		int	nDay;
		int	nHour;
		int	nMinute;
		double	dSecond;

		double	ApproximatedNormalizedSeconds() const
		{
			return ((((nYear*12+nMonth)*31+nDay)*24+nHour)*60+nMinute)*60+dSecond;
		}
	};

	struct CTime : public CDateTimeBase
	{
		CTime(
			int	_nHour = 0,
			int	_nMinute = 0,
			double	_dSecond = 0.0
		)
			: nHour( _nHour )
			, nMinute( _nMinute )
			, dSecond( _dSecond )
		{};

		CTime(
			int	_nHour,
			int	_nMinute,
			double	_dSecond,
			int _nTimezoneOffset
		)
			: nHour( _nHour )
			, nMinute( _nMinute )
			, dSecond( _dSecond )
		{
			SetTimezoneOffset( _nTimezoneOffset );
		};

		int	nHour;
		int	nMinute;
		double	dSecond;

		double NormalizedSeconds() const		{ return nHour * 3600 + nMinute * 60 + dSecond - nOffset * 60; }
	};

	struct CYearMonth : public CDateTimeBase
	{
		CYearMonth()
		{
			time_t long_time;
			time( &long_time ); 
			struct tm *newtime = localtime( &long_time ); 
			nYear = newtime->tm_year + 1900;
			nMonth = newtime->tm_mon + 1;
		};

		CYearMonth(
			int	_nYear,
			int	_nMonth
		)
			: nYear( _nYear )
			, nMonth( _nMonth )
		{};

		int	nYear;
		int	nMonth;

		long	NormalizedMonths() const { return nYear*12+nMonth; }
	};

	struct CMonthDay : public CDateTimeBase
	{
		CMonthDay()
		{
			time_t long_time;
			time( &long_time ); 
			struct tm *newtime = localtime( &long_time ); 
			nMonth = newtime->tm_mon + 1;
			nDay = newtime->tm_mday;
		};

		CMonthDay(
			int	_nMonth,
			int	_nDay
		)
			: nMonth( _nMonth )
			, nDay( _nDay )
		{};

		int	nMonth;
		int	nDay;

		long ApproximatedNormalizedDays() const	{ return nMonth*31+nDay; }
	};

	struct CYear : public CDateTimeBase
	{
		CYear()
		{
			time_t long_time;
			time( &long_time ); 
			struct tm *newtime = localtime( &long_time ); 
			nYear = newtime->tm_year + 1900;
		};

		CYear( int _nYear )	: nYear( _nYear ) {};

		int		nYear;
	};

	struct CMonth : public CDateTimeBase
	{
		CMonth()
		{
			time_t long_time;
			time( &long_time ); 
			struct tm *newtime = localtime( &long_time ); 
			nMonth = newtime->tm_mon + 1;
		};

		CMonth( int _nMonth ) : nMonth( _nMonth ) {};

		int		nMonth;
	};

	struct CDay : public CDateTimeBase
	{
		CDay()
		{
			time_t long_time;
			time( &long_time ); 
			struct tm *newtime = localtime( &long_time ); 
			nDay = newtime->tm_sec;
		};

		CDay( int _nDay ) : nDay( _nDay ) {};

		int		nDay;
	};


	// ---------- Interface ----------
	virtual operator tstring() const = 0;

	virtual operator CDuration() const = 0;
	virtual operator CTime() const = 0;
	virtual operator CDay() const = 0;
	virtual operator CMonth() const = 0;
	virtual operator CMonthDay() const = 0;
	virtual operator CYear() const = 0;
	virtual operator CYearMonth() const = 0;
	virtual operator CDate() const = 0;
	virtual operator CDateTime() const = 0;

	virtual CSchemaType::ETypes CalendarType() const = 0;
	bool	IsConvertableTo(CSchemaType::ETypes eType) const;

	// --------- Helper functions and utilities ---------
	static void				DateTimeAddDuration( int& nYear, int& nMonth, int& nDay, int& nHour, int& nMinute, double& dSecond, const CDuration& dur);

protected:
	static int				ParseInt( const TCHAR** pszData, const TCHAR* szEstimatedType, const TCHAR* szValue, const int nMin = INT_MIN, const int nMax = INT_MAX );
	static double			ParseDouble( const TCHAR** pszData, const TCHAR* szEstimatedType, const TCHAR* szValue );
	static void				ParseChar( const TCHAR** pszData, TCHAR chValue, const TCHAR* szEstimatedType, const TCHAR* szValue );

	static inline double	fQuotient(double a, double b);
	static inline double	fQuotient(double a, double low, double high);
	static inline double	modulo(double a, double b);
	static inline double	modulo(double a, double low, double high);
	static inline bool		isLeapYear(int year);
	static int				maxDayInMonthFor(int year, int month);

	static int				WriteTime( TCHAR* szTarget, int nHour, int nMinute, double dSecond );
	static void				ParseTZ( CDateTimeBase& rTimezone, const TCHAR* szValue );
	static void				WriteTZ( const CDateTimeBase& rTimezone, TCHAR* szTarget );
};


////////////////////////////////////////////////////////////////////////
//
//  template CSchemaCalendar<...>
//
////////////////////////////////////////////////////////////////////////

template <typename TValue, CSchemaType::ETypes eCalendarType> 
class CSchemaCalendar : public CSchemaTypeCalendar, public CSchemaType
{
public:
	// Construction
	CSchemaCalendar()																{};
	CSchemaCalendar( const TValue value ) : m_Value( value )						{ SetIsEmpty( false ); };
	CSchemaCalendar( const TCHAR* szValue )											{ Parse( szValue ); }
	CSchemaCalendar( const CSchemaType& rOther )									{ Assign( rOther ); }


	// Operations
	TValue&									GetValue()								{ return m_Value; }
	const TValue&							GetValue() const						{ return m_Value; }
	void									SetValue( TValue value )				{ m_Value = value; SetIsEmpty( false ); }

	virtual CSchemaCalendar& operator=( const CSchemaType& rOther )					{ return ( CSchemaCalendar& )Assign( rOther ); }

	// --------- Interface CSchemaType ----------
	friend class CSchemaType;
	// Operators
	virtual bool							ToBool() const							{ return !IsEmpty(); }
	virtual operator						tstring() const							{ return IsEmpty() ? _T("") : ToString(); }

	// Operations
	virtual void							Empty()
	{
		CSchemaType::Empty();
		m_Value = TValue();
	}

	virtual bool							Parse( const TCHAR* szValue )
	{
		if( !CSchemaType::Parse( szValue ) )
			return false;

		InternalDTParse( szValue );

		return !IsEmpty();
	}

	virtual CSchemaType&	Assign( const CSchemaType& rOther )
	{
		if( CSchemaType::Assign( rOther ).IsEmpty() )
			return *this;

		const CSchemaTypeCalendar* pCalendarType = dynamic_cast<const CSchemaTypeCalendar*>( &rOther );
		if( pCalendarType == NULL)
			ThrowIncompatibleTypesError();

		m_Value = *pCalendarType;

		return *this;
	}

	// ---------- Interface CSchemaTypeCalendar ----------
	virtual ETypes							CalendarType() const						{ return ( CSchemaType::ETypes )TYPE; }
	virtual operator						CSchemaTypeCalendar::CDuration() const		{ ThrowIncompatibleTypesError(); }
	virtual operator						CSchemaTypeCalendar::CDate() const			{ ThrowIncompatibleTypesError(); }
	virtual operator						CSchemaTypeCalendar::CTime() const			{ ThrowIncompatibleTypesError(); }
	virtual operator						CSchemaTypeCalendar::CDateTime() const		{ ThrowIncompatibleTypesError(); }
	virtual operator						CSchemaTypeCalendar::CYearMonth() const		{ ThrowIncompatibleTypesError(); }
	virtual operator						CSchemaTypeCalendar::CYear() const			{ ThrowIncompatibleTypesError(); }
	virtual operator						CSchemaTypeCalendar::CMonth() const			{ ThrowIncompatibleTypesError(); }
	virtual operator						CSchemaTypeCalendar::CMonthDay() const		{ ThrowIncompatibleTypesError(); }
	virtual operator						CSchemaTypeCalendar::CDay() const			{ ThrowIncompatibleTypesError(); }

	enum { TYPE = eCalendarType };

	typedef const TCHAR*					basetype;

protected:
	tstring									ToString() const							{ return _T(""); }
	void									InternalDTParse( const TCHAR* szValue )		{}

	TValue									m_Value;
};


////////////////////////////////////////////////////////////////////////
//
//  CSchemaDuration
//  CSchemaDate
//  CSchemaTime
//  CSchemaDateTime
//  CSchemaYear
//  CSchemaYearMonth
//  CSchemaMonth
//  CSchemaMonthDay
//  CSchemaDay
//
////////////////////////////////////////////////////////////////////////


typedef CSchemaCalendar< CSchemaTypeCalendar::CDuration, CSchemaType::k_Duration >	CSchemaDuration;
typedef CSchemaCalendar< CSchemaTypeCalendar::CDate,		CSchemaType::k_Date >		CSchemaDate;
typedef CSchemaCalendar< CSchemaTypeCalendar::CTime,		CSchemaType::k_Time >		CSchemaTime;
typedef CSchemaCalendar< CSchemaTypeCalendar::CDateTime, CSchemaType::k_DateTime >	CSchemaDateTime;
typedef CSchemaCalendar< CSchemaTypeCalendar::CYear,		CSchemaType::k_Year >		CSchemaYear;
typedef CSchemaCalendar< CSchemaTypeCalendar::CYearMonth,CSchemaType::k_YearMonth > CSchemaYearMonth;
typedef CSchemaCalendar< CSchemaTypeCalendar::CMonth,	CSchemaType::k_Month >		CSchemaMonth;
typedef CSchemaCalendar< CSchemaTypeCalendar::CMonthDay, CSchemaType::k_MonthDay >	CSchemaMonthDay;
typedef CSchemaCalendar< CSchemaTypeCalendar::CDay,		CSchemaType::k_Day >		CSchemaDay;

#define	CSchemaDate_BASETYPE_LPCTSTR
#define CSchemaDateTime_BASETYPE_LPCTSTR
#define	CSchemaTime_BASETYPE_LPCTSTR
#define	CSchemaDateTime_BASETYPE_LPCTSTR
#define	CSchemaYear_BASETYPE_LPCTSTR
#define	CSchemaYearMonth_BASETYPE_LPCTSTR
#define	CSchemaMonth_BASETYPE_LPCTSTR
#define	CSchemaMonthDay_BASETYPE_LPCTSTR
#define	CSchemaDay_BASETYPE_LPCTSTR
#define	CSchemaString_BASETYPE_LPCTSTR

////////////////////////////////////////////////////////////////////////
//
//  CSchemaDuration
//
////////////////////////////////////////////////////////////////////////
template<>
inline void CSchemaDuration::InternalDTParse( const TCHAR* szValue )
{
	const TCHAR* szTmp = szValue;
	bool bTime = false;
	int nTmp;

	if( !(m_Value.bPositive = *szTmp != _T('-')) )
		szTmp++;
	int nSign = m_Value.bPositive ? 1 : -1;
	ParseChar(&szTmp, _T('P'), _T("duration"), szValue);

	while (*szTmp)
	{
		if (*szTmp == _T('T'))
		{
			if (bTime)
				ThrowFormatError();
			szTmp++;
			bTime = true;
		}
		nTmp = ParseInt(&szTmp, _T("duration"), szValue);
		if (!bTime)
		{
			switch (*szTmp)
			{
			case _T('Y'):	m_Value.nYear  = nTmp * nSign; break;
			case _T('M'):	m_Value.nMonth = nTmp * nSign; break;
			case _T('D'):	m_Value.nDay   = nTmp * nSign; break;
			default:		ThrowFormatError();
			}
		}
		else
		{
			switch (*szTmp)
			{
			case _T('H'):	m_Value.nHour   = nTmp * nSign; break;
			case _T('M'):	m_Value.nMinute = nTmp * nSign; break;
			case _T('S'):	m_Value.dSecond = nTmp * nSign; break;
			case _T('.'):	m_Value.dSecond = (nTmp + ParseDouble(&szTmp, _T("duration"), szValue)) * nSign;
							if (*szTmp != _T('S'))
								ThrowFormatError();
							break;
			default:		ThrowFormatError();
			}
		}
		szTmp++;
	}
}


template<>
inline tstring CSchemaDuration::ToString() const
{
	TCHAR szResult[64];
	TCHAR* szTmp = szResult;

	if (!m_Value.bPositive)
		*szTmp++ = _T('-');
	*szTmp++ = _T('P');
	if (m_Value.nYear != 0)
		szTmp += _stprintf(szTmp, _T("%iY"), abs( m_Value.nYear ));
	if (m_Value.nMonth != 0)
		szTmp += _stprintf(szTmp, _T("%iM"), abs( m_Value.nMonth ));
	if (m_Value.nDay != 0)
		szTmp += _stprintf(szTmp, _T("%iD"), abs( m_Value.nDay ));
	if (m_Value.nHour != 0 || m_Value.nMinute != 0 || m_Value.dSecond != 0)
	{
		*szTmp++ = _T('T');
		if (m_Value.nHour != 0)
			szTmp += _stprintf(szTmp, _T("%iH"), abs( m_Value.nHour ));
		if (m_Value.nMinute != 0)
			szTmp += _stprintf(szTmp, _T("%iM"), abs( m_Value.nMinute ));
		if (m_Value.dSecond != 0)
		{
			double d = m_Value.dSecond < 0 ? m_Value.dSecond * -1 : m_Value.dSecond;
			if (m_Value.dSecond - floor(m_Value.dSecond) == 0)
			{
				szTmp += _stprintf(szTmp, _T("%01.f"), d);
			}
			else
			{
				szTmp += _stprintf(szTmp, _T("%.9f"), d);
				while (*(szTmp-1) == _T('0'))
					*--szTmp = 0;
			}
			*szTmp++ = _T('S');
		}
	}
	*szTmp = 0;

	return szResult;
}


template<>
inline CSchemaDuration::operator CSchemaTypeCalendar::CDuration() const
{
	return m_Value;
}


template<>
inline CSchemaDuration::operator CSchemaTypeCalendar::CTime() const
{
	if( IsEmpty() )
		return CSchemaTypeCalendar::CTime();

	if( m_Value.nDay>0 || m_Value.nMonth>0 || m_Value.nYear>0 || !m_Value.bPositive )
		ThrowValuesNotConvertableError();

	CSchemaTypeCalendar::CTime dt;
	dt.nHour = m_Value.nHour;
	dt.nMinute = m_Value.nMinute;
	dt.dSecond = m_Value.dSecond;
	return dt;
}


////////////////////////////////////////////////////////////////////////
//
//  CSchemaDate
//
////////////////////////////////////////////////////////////////////////
template<>
inline void CSchemaDate::InternalDTParse( const TCHAR* szValue )
{
	const TCHAR* szTmp = szValue;
	m_Value.nYear = ParseInt( &szTmp, _T("date"), szValue, 0, 9999 );
	ParseChar( &szTmp, _T('-'), _T("date"), szValue );
	m_Value.nMonth = ParseInt( &szTmp, _T("date"), szValue, 1, 12 );
	ParseChar( &szTmp, _T('-'), _T("date"), szValue );
	m_Value.nDay = ParseInt( &szTmp, _T("date"), szValue, 1, 31 );
	ParseTZ( m_Value, szTmp);
}


template<>
inline tstring CSchemaDate::ToString() const
{
	TCHAR szTmp[32];
	int nPos = _stprintf( szTmp, _T("%04i-%02i-%02i"), m_Value.nYear, m_Value.nMonth, m_Value.nDay );
	WriteTZ( m_Value, szTmp + nPos );
	return szTmp;
}


template<>
inline CSchemaDate::operator CSchemaTypeCalendar::CDate() const
{
	return m_Value;
}


template<>
inline CSchemaDate::operator CSchemaTypeCalendar::CDateTime() const
{
	if( IsEmpty() )
		return CSchemaTypeCalendar::CDateTime();

	CSchemaTypeCalendar::CDateTime	dt;
	dt.nYear = m_Value.nYear;
	dt.nMonth = m_Value.nMonth;
	dt.nDay = m_Value.nDay;
	dt.nTZMode = m_Value.nTZMode;
	dt.nOffset = m_Value.nOffset;
	return dt;
}

template<>
inline CSchemaDate::operator CSchemaTypeCalendar::CDay() const
{
	if( IsEmpty() )
		return CSchemaTypeCalendar::CDay();

	CSchemaTypeCalendar::CDay	dt;
	dt.nDay = m_Value.nDay;
	dt.nTZMode = m_Value.nTZMode;
	dt.nOffset = m_Value.nOffset;
	return dt;
}

template<>
inline CSchemaDate::operator CSchemaTypeCalendar::CMonth() const
{
	if( IsEmpty() )
		return CSchemaTypeCalendar::CMonth();

	CSchemaTypeCalendar::CMonth	dt;
	dt.nMonth = m_Value.nMonth;
	dt.nTZMode = m_Value.nTZMode;
	dt.nOffset = m_Value.nOffset;
	return dt;
}

template<>
inline CSchemaDate::operator CSchemaTypeCalendar::CMonthDay() const
{
	if( IsEmpty() )
		return CSchemaTypeCalendar::CMonthDay();

	CSchemaTypeCalendar::CMonthDay	dt;
	dt.nMonth = m_Value.nMonth;
	dt.nDay = m_Value.nDay;
	dt.nTZMode = m_Value.nTZMode;
	dt.nOffset = m_Value.nOffset;
	return dt;
}

template<>
inline CSchemaDate::operator CSchemaTypeCalendar::CYear() const
{
	if( IsEmpty() )
		return CSchemaTypeCalendar::CYear();

	CSchemaTypeCalendar::CYear	dt;
	dt.nYear = m_Value.nYear;
	dt.nTZMode = m_Value.nTZMode;
	dt.nOffset = m_Value.nOffset;
	return dt;
}

template<>
inline CSchemaDate::operator CSchemaTypeCalendar::CYearMonth() const
{
	if( IsEmpty() )
		return CSchemaTypeCalendar::CYearMonth();

	CSchemaTypeCalendar::CYearMonth	dt;
	dt.nYear = m_Value.nYear;
	dt.nMonth = m_Value.nMonth;
	dt.nTZMode = m_Value.nTZMode;
	dt.nOffset = m_Value.nOffset;
	return dt;
}


////////////////////////////////////////////////////////////////////////
//
//  CSchemaTime
//
////////////////////////////////////////////////////////////////////////
template<>
inline void CSchemaTime::InternalDTParse( const TCHAR* szValue )
{
	const TCHAR* szTmp = szValue;
	m_Value.nHour = ParseInt( &szTmp, _T("time"), szValue, 0, 23 );
	ParseChar( &szTmp, _T(':'), _T("time"), szValue );
	m_Value.nMinute = ParseInt( &szTmp, _T("time"), szValue, 0, 59 );
	ParseChar( &szTmp, _T(':'), _T("time"), szValue );
	m_Value.dSecond = ParseDouble( &szTmp, _T("time"), szValue );
	ParseTZ( m_Value, szTmp);
}


template<>
inline tstring CSchemaTime::ToString() const
{
	TCHAR szTmp[64];
	int nPos = WriteTime(szTmp, m_Value.nHour, m_Value.nMinute, m_Value.dSecond);
	WriteTZ( m_Value, szTmp + nPos);
	return szTmp;
}


template<>
inline CSchemaTime::operator CSchemaTypeCalendar::CTime() const
{
	return m_Value;
}


template<>
inline CSchemaTime::operator CSchemaTypeCalendar::CDuration() const
{
	if( IsEmpty() )
		return CSchemaTypeCalendar::CDuration();

	CSchemaTypeCalendar::CDuration	dur;
	dur.nHour = m_Value.nHour;
	dur.nMinute = m_Value.nMinute;
	dur.dSecond = m_Value.dSecond;
	return dur;
}


////////////////////////////////////////////////////////////////////////
//
//  CSchemaDateTime
//
////////////////////////////////////////////////////////////////////////


template<>
inline void CSchemaDateTime::InternalDTParse( const TCHAR* szValue )
{
	const TCHAR* szTmp = szValue;
	m_Value.nYear = ParseInt( &szTmp, _T("datetime"), szValue, 0, 9999 );
	ParseChar( &szTmp, _T('-'), _T("datetime"), szValue );
	m_Value.nMonth = ParseInt( &szTmp, _T("datetime"), szValue, 1, 12 );
	ParseChar( &szTmp, _T('-'), _T("datetime"), szValue );
	m_Value.nDay = ParseInt( &szTmp, _T("datetime"), szValue, 1, 31 );
	ParseChar( &szTmp, _T('T'), _T("datetime"), szValue );
	m_Value.nHour = ParseInt( &szTmp, _T("datetime"), szValue, 0, 23 );
	ParseChar( &szTmp, _T(':'), _T("datetime"), szValue );
	m_Value.nMinute = ParseInt( &szTmp, _T("datetime"), szValue, 0, 59 );
	ParseChar( &szTmp, _T(':'), _T("datetime"), szValue );
	m_Value.dSecond = ParseDouble( &szTmp, _T("datetime"), szValue );
	ParseTZ( m_Value, szTmp );
}


template<>
inline tstring CSchemaDateTime::ToString() const
{
	TCHAR szTmp[64];
	int nPos = _stprintf(szTmp, _T("%04i-%02i-%02iT"), m_Value.nYear, m_Value.nMonth, m_Value.nDay);
	nPos += WriteTime( szTmp + nPos, m_Value.nHour, m_Value.nMinute, m_Value.dSecond);
	WriteTZ( m_Value, szTmp + nPos);
	return szTmp;
}





template<>
inline CSchemaDateTime::operator CSchemaTypeCalendar::CDateTime() const
{
	return m_Value;
}


template<>
inline CSchemaDateTime::operator CSchemaTypeCalendar::CDate() const
{
	if( IsEmpty() )
		return CSchemaTypeCalendar::CDate();

	CSchemaTypeCalendar::CDate	dt;
	dt.nYear = m_Value.nYear;
	dt.nMonth = m_Value.nMonth;
	dt.nDay = m_Value.nDay;
	dt.nTZMode = m_Value.nTZMode;
	dt.nOffset = m_Value.nOffset;
	return dt;
}


template<>
inline CSchemaDateTime::operator CSchemaTypeCalendar::CDay() const
{
	if( IsEmpty() )
		return CSchemaTypeCalendar::CDay();

	CSchemaTypeCalendar::CDay	dt;
	dt.nDay = m_Value.nDay;
	dt.nTZMode = m_Value.nTZMode;
	dt.nOffset = m_Value.nOffset;
	return dt;
}


template<>
inline CSchemaDateTime::operator CSchemaTypeCalendar::CMonth() const
{
	if( IsEmpty() )
		return CSchemaTypeCalendar::CMonth();

	CSchemaTypeCalendar::CMonth	dt;
	dt.nMonth = m_Value.nMonth;
	dt.nTZMode = m_Value.nTZMode;
	dt.nOffset = m_Value.nOffset;
	return dt;
}


template<>
inline CSchemaDateTime::operator CSchemaTypeCalendar::CMonthDay() const
{
	if( IsEmpty() )
		return CSchemaTypeCalendar::CMonthDay();

	CSchemaTypeCalendar::CMonthDay	dt;
	dt.nMonth = m_Value.nMonth;
	dt.nDay = m_Value.nDay;
	dt.nTZMode = m_Value.nTZMode;
	dt.nOffset = m_Value.nOffset;
	return dt;
}


template<>
inline CSchemaDateTime::operator CSchemaTypeCalendar::CTime() const
{
	if( IsEmpty() )
		return CSchemaTypeCalendar::CTime();

	CSchemaTypeCalendar::CTime	dt;
	dt.nHour = m_Value.nHour;
	dt.nMinute = m_Value.nMinute;
	dt.dSecond = m_Value.dSecond;
	dt.nTZMode = m_Value.nTZMode;
	dt.nOffset = m_Value.nOffset;
	return dt;
}


template<>
inline CSchemaDateTime::operator CSchemaTypeCalendar::CYear() const
{
	if( IsEmpty() )
		return CSchemaTypeCalendar::CYear();

	CSchemaTypeCalendar::CYear	dt;
	dt.nYear = m_Value.nYear;
	dt.nTZMode = m_Value.nTZMode;
	dt.nOffset = m_Value.nOffset;
	return dt;
}


template<>
inline CSchemaDateTime::operator CSchemaTypeCalendar::CYearMonth() const
{
	if( IsEmpty() )
		return CSchemaTypeCalendar::CYearMonth();

	CSchemaTypeCalendar::CYearMonth	dt;
	dt.nYear = m_Value.nYear;
	dt.nMonth = m_Value.nMonth;
	dt.nTZMode = m_Value.nTZMode;
	dt.nOffset = m_Value.nOffset;
	return dt;
}


////////////////////////////////////////////////////////////////////////
//
//  CSchemaYear
//
////////////////////////////////////////////////////////////////////////
template<>
inline void CSchemaYear::InternalDTParse( const TCHAR* szValue )
{
	const TCHAR* szTmp = szValue;
	m_Value.nYear = ParseInt( &szTmp, _T("gYear"), szValue, 0, 9999 );
	ParseTZ( m_Value, szTmp);
}


template<>
inline tstring CSchemaYear::ToString() const
{
	TCHAR szTmp[32];
	int nPos = _stprintf(szTmp, _T("%04i"), m_Value.nYear);
	WriteTZ( m_Value, szTmp + nPos );
	return szTmp;
}


template<>
inline CSchemaYear::operator CSchemaTypeCalendar::CYear() const
{
	return m_Value;
}


////////////////////////////////////////////////////////////////////////
//
//  CSchemaYearMonth
//
////////////////////////////////////////////////////////////////////////
template<>
inline void CSchemaYearMonth::InternalDTParse( const TCHAR* szValue )
{
	const TCHAR* szTmp = szValue;
	m_Value.nYear = ParseInt( &szTmp, _T("gYearMonth"), szValue, 0, 9999 );
	ParseChar( &szTmp, _T('-'), _T("gYearMonth"), szValue );
	m_Value.nMonth = ParseInt( &szTmp, _T("gYearMonth"), szValue, 1, 12 );
	ParseTZ( m_Value, szTmp );
}


template<>
inline tstring CSchemaYearMonth::ToString() const
{
	TCHAR szTmp[32];
	int nPos = _stprintf(szTmp, _T("%04i-%02i"), m_Value.nYear, m_Value.nMonth);
	WriteTZ( m_Value, szTmp + nPos);
	return szTmp;
}


template<>
inline CSchemaYearMonth::operator CSchemaTypeCalendar::CYearMonth() const
{
	return m_Value;
}


template<>
inline CSchemaYearMonth::operator CSchemaTypeCalendar::CMonth() const
{
	if( IsEmpty() )
		return CSchemaTypeCalendar::CMonth();

	CSchemaTypeCalendar::CMonth	dt;
	dt.nMonth = m_Value.nMonth;
	dt.nTZMode = m_Value.nTZMode;
	dt.nOffset = m_Value.nOffset;
	return dt;
}


template<>
inline CSchemaYearMonth::operator CSchemaTypeCalendar::CYear() const
{
	if( IsEmpty() )
		return CSchemaTypeCalendar::CYear();

	CSchemaTypeCalendar::CYear	dt;
	dt.nYear = m_Value.nYear;
	dt.nTZMode = m_Value.nTZMode;
	dt.nOffset = m_Value.nOffset;
	return dt;
}


////////////////////////////////////////////////////////////////////////
//
//  CSchemaMonth
//
////////////////////////////////////////////////////////////////////////
template<>
inline void CSchemaMonth::InternalDTParse( const TCHAR* szValue )
{
	const TCHAR* szTmp = szValue;
	ParseChar( &szTmp, _T('-'), _T("gMonth"), szValue );
	ParseChar( &szTmp, _T('-'), _T("gMonth"), szValue );
	m_Value.nMonth = ParseInt( &szTmp, _T("gMonth"), szValue, 1, 12 );
	ParseChar( &szTmp, _T('-'), _T("gMonth"), szValue );
	ParseChar( &szTmp, _T('-'), _T("gMonth"), szValue );
	ParseTZ( m_Value, szTmp );
}


template<>
inline tstring CSchemaMonth::ToString() const
{
	TCHAR szTmp[32];
	int nPos = _stprintf(szTmp, _T("--%02i--"), m_Value.nMonth);
	WriteTZ( m_Value, szTmp + nPos );
	return szTmp;
}


template<>
inline CSchemaMonth::operator CSchemaTypeCalendar::CMonth() const
{
	return m_Value;
}


////////////////////////////////////////////////////////////////////////
//
//  CSchemaMonthDay
//
////////////////////////////////////////////////////////////////////////
template<>
inline void CSchemaMonthDay::InternalDTParse( const TCHAR* szValue )
{
	const TCHAR* szTmp = szValue;
	ParseChar( &szTmp, _T('-'), _T("gMonthDay"), szValue );
	ParseChar( &szTmp, _T('-'), _T("gMonthDay"), szValue );
	m_Value.nMonth = ParseInt( &szTmp, _T("gMonthDay"), szValue, 1, 12 );
	ParseChar( &szTmp, _T('-'), _T("gMonthDay"), szValue );
	m_Value.nDay = ParseInt( &szTmp, _T("gMonthDay"), szValue, 1, 31 );
	ParseTZ( m_Value, szTmp );
}


template<>
inline tstring CSchemaMonthDay::ToString() const
{
	TCHAR szTmp[32];
	int nPos = _stprintf( szTmp, _T("--%02i-%02i"), m_Value.nMonth, m_Value.nDay );
	WriteTZ( m_Value, szTmp + nPos);
	return szTmp;
}


template<>
inline CSchemaMonthDay::operator CSchemaTypeCalendar::CMonthDay() const
{
	return m_Value;
}


template<>
inline CSchemaMonthDay::operator CSchemaTypeCalendar::CDay() const
{
	if( IsEmpty() )
		return CSchemaTypeCalendar::CDay();

	CSchemaTypeCalendar::CDay	dt;
	dt.nDay = m_Value.nDay;
	dt.nTZMode = m_Value.nTZMode;
	dt.nOffset = m_Value.nOffset;
	return dt;
}


template<>
inline CSchemaMonthDay::operator CSchemaTypeCalendar::CMonth() const
{
	if( IsEmpty() )
		return CSchemaTypeCalendar::CMonth();

	CSchemaTypeCalendar::CMonth	dt;
	dt.nMonth = m_Value.nMonth;
	dt.nTZMode = m_Value.nTZMode;
	dt.nOffset = m_Value.nOffset;
	return dt;
}


////////////////////////////////////////////////////////////////////////
//
//  CSchemaDay
//
////////////////////////////////////////////////////////////////////////
template<>
inline void CSchemaDay::InternalDTParse( const TCHAR* szValue )
{
	const TCHAR* szTmp = szValue;
	ParseChar( &szTmp, _T('-'), _T("gDay"), szValue );
	ParseChar( &szTmp, _T('-'), _T("gDay"), szValue );
	ParseChar( &szTmp, _T('-'), _T("gDay"), szValue );
	m_Value.nDay = ParseInt( &szTmp, _T("gDay"), szValue, 1, 31 );
	ParseTZ( m_Value, szTmp );
}


template<>
inline tstring CSchemaDay::ToString() const
{
	TCHAR szTmp[32];
	int nPos = _stprintf(szTmp, _T("---%02i"), m_Value.nDay);
	WriteTZ( m_Value, szTmp + nPos );
	return szTmp;
}


template<>
inline CSchemaDay::operator CSchemaTypeCalendar::CDay() const
{
	return m_Value;
}


////////////////////////////////////////////////////////////////////////
//
//  Operators
//
////////////////////////////////////////////////////////////////////////

bool			ALTOVA_DECLSPECIFIER operator ==( const CSchemaDate& t1, const CSchemaDate& t2 );
bool 			ALTOVA_DECLSPECIFIER operator <	( const CSchemaDate& t1, const CSchemaDate& t2 );
bool 			ALTOVA_DECLSPECIFIER operator !=( const CSchemaDate& t1, const CSchemaDate& t2 );
bool 			ALTOVA_DECLSPECIFIER operator <=( const CSchemaDate& t1, const CSchemaDate& t2 );
bool 			ALTOVA_DECLSPECIFIER operator >	( const CSchemaDate& t1, const CSchemaDate& t2 );
bool 			ALTOVA_DECLSPECIFIER operator >=( const CSchemaDate& t1, const CSchemaDate& t2 );

bool 			ALTOVA_DECLSPECIFIER operator ==( const CSchemaTime& t1, const CSchemaTime& t2 );
bool 			ALTOVA_DECLSPECIFIER operator <	( const CSchemaTime& t1, const CSchemaTime& t2 );
bool 			ALTOVA_DECLSPECIFIER operator !=( const CSchemaTime& t1, const CSchemaTime& t2 );
bool 			ALTOVA_DECLSPECIFIER operator <=( const CSchemaTime& t1, const CSchemaTime& t2 );
bool 			ALTOVA_DECLSPECIFIER operator >	( const CSchemaTime& t1, const CSchemaTime& t2 );
bool 			ALTOVA_DECLSPECIFIER operator >=( const CSchemaTime& t1, const CSchemaTime& t2 );

bool 			ALTOVA_DECLSPECIFIER operator ==( const CSchemaDateTime& t1, const CSchemaDateTime& t2 );
bool 			ALTOVA_DECLSPECIFIER operator <	( const CSchemaDateTime& t1, const CSchemaDateTime& t2 );
bool 			ALTOVA_DECLSPECIFIER operator !=( const CSchemaDateTime& t1, const CSchemaDateTime& t2 );
bool 			ALTOVA_DECLSPECIFIER operator <=( const CSchemaDateTime& t1, const CSchemaDateTime& t2 );
bool 			ALTOVA_DECLSPECIFIER operator >	( const CSchemaDateTime& t1, const CSchemaDateTime& t2 );
bool 			ALTOVA_DECLSPECIFIER operator >=( const CSchemaDateTime& t1, const CSchemaDateTime& t2 );

CSchemaDuration	ALTOVA_DECLSPECIFIER operator +	( const CSchemaDuration& dur1, const CSchemaDuration& dur2 );
CSchemaDate		ALTOVA_DECLSPECIFIER operator +	( const CSchemaDate& t, const CSchemaDuration& dur );
CSchemaDuration	ALTOVA_DECLSPECIFIER operator -	( const CSchemaDate& t1, const CSchemaDate& t2 );
CSchemaTime		ALTOVA_DECLSPECIFIER operator +	( const CSchemaTime& t, const CSchemaDuration& dur );
CSchemaDuration	ALTOVA_DECLSPECIFIER operator -	( const CSchemaTime& t1, const CSchemaTime& t2 );
CSchemaDateTime	ALTOVA_DECLSPECIFIER operator +	( const CSchemaDateTime& t, const CSchemaDuration& dur );
CSchemaDuration	ALTOVA_DECLSPECIFIER operator -	( const CSchemaDateTime& t1, const CSchemaDateTime& t2 );


} // namespace altova

#endif // ALTOVA_SCHEMATYPE_CALENDAR_H_INCLUDED
