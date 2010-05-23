/*
 * CalTime.hpp
 *
 *  Created on: 12.01.2010
 *      Author: cyborg-x1
 */


//Some overall TODOs
//TODO: [DOC] Add Doxygen Documentation
//TODO: [DOC] Add a notice to Doxygen that this should not be used as historical reference...
//TODO: [!Function] Stringstream Output and Formatting




#ifndef CALTIME_HPP_
#define CALTIME_HPP_

#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <boost/tokenizer.hpp>
#include <math.h>

#ifndef _PARSE_LIMIT_YEAR_
	#define _PARSE_LIMIT_YEAR_ 4
#endif
#ifndef _PARSE_LIMIT_TIMERANGE_ //FOR EACH PART (days minute seconds)
	#define _PARSE_LIMIT_TIMERANGE_ 9
#endif


namespace LibCalTime {
using namespace std;
using namespace boost;

typedef unsigned int year_t;
typedef unsigned int month_t;
typedef unsigned int day_t;
typedef unsigned int hour_t;
typedef unsigned int minute_t;
typedef unsigned int second_t;
typedef long timezone_t;


typedef struct
{
	year_t year;
	month_t month;
	day_t day;
	hour_t hour;
	minute_t minute;
	second_t second;
}date_set;


enum LibTime_Errors
{
	LCTERR_I_PARSE_ERROR,
	LCTERR_SETTIMEVALUE_WRONG_MODE,
	LCTERR_ISNULL_WRONGMODE,
	LCTERR_NEG_YEAR,
	LCTERR_UNK_TSM,
	LCTERR_INCDEC_WRONGMODE,
	LCTERR_TRADDSUBELEM_WRONGMODE,
	LCTERR_DATEVALUE_OUT_OF_RANGE,
	LCTERR_DATE_NOT_POSSIBLE,
	LCTERR_PRPOUTTIME_MODE,
	LCTERR_DATES_ADD,
	LCTERR_ADDSUB_TRD_WRONGMODE,
	LCTERR_EMPT_TOKKEN,
	LCTERR_TRANGE_STR,
	LCTERR_NO_PLUSMINUS,
	LCTERR_TRANGE_NO_DHMS,
	LCTERR_UNALLOWED_CHAR_LENGTH,
	LCTERR_TRANGE_NO_UNIT,
	LCTERR_RANGE_DATE_COMPARISON,
	LCTERR_TIMEZONE_OOR,
};

class CalTimeException : public std::runtime_error
{
int error_number;
public:
	CalTimeException(string what, int err_no) : std::runtime_error(what)
	{
		this->error_number=err_no;
	}
};

class CalTimeRangeError : public std::range_error
{
int error_number;
public:
	CalTimeRangeError(string what, int err_no) : std::range_error(what)
	{
		this->error_number=err_no;
	}
};




enum Errors
{
	ERR_MonthOutOfRange = -1,
};

enum TimeSetupMode
{
	TSM_NOW,
	TSM_TIME_RANGE
};

enum TimeMember
{
	YEAR,
	MONTH,
	DAY,
	HOUR,
	MINUTE,
	SECOND,
};

enum TimeZone
{
	//Australia
	AUS_ACDT	=37800,
	AUS_ACST	=34200,
	AUS_AEDT	=39600,
	AUS_AEST	=36000,
	AUS_AWDT	=32400,
	AUS_AWST	=28800,
	AUS_CDT		=37800,
	AUS_CSuT	=37800,
	AUS_CStT	=34200,
	AUS_CXT		=25200,
	AUS_EDT		=39600,
	AUS_ESuT	=39600,
	AUS_EStT	=36000,
	AUS_NFT		=41400,
	AUS_WDT		=32400,
	AUS_WSuT	=32400,
	AUS_WStT	=28800,
	//Europe
	EUR_BST		=3600,
	EUR_CEDT	=7200,
	EUR_CEST	=7200,
	EUR_CET		=3600,
	EUR_EEDT	=10800,
	EUR_EEST	=10800,
	EUR_EET		=7200,
	EUR_GMT		=0,
	EUR_IST		=3600,
	EUR_MESZ	=7200,
	EUR_MEZ		=3600,
	EUR_MSD		=14400,
	EUR_MSK		=10800,
	EUR_UTC		=0,
	EUR_WEDT	=3600,
	EUR_WEST	=3600,
	EUR_WET		=0,
	//Military
	MIL_ALPHA	=3600,
	MIL_BRAVO	=7200,
	MIL_CHARLIE	=10800,
	MIL_DELTA	=14400,
	MIL_ECHO	=18000,
	MIL_FOXTROT	=21600,
	MIL_GOLF	=25200,
	MIL_HOTEL	=28800,
	MIL_INDIA	=32400,
	MIL_KILO	=36000,
	MIL_LIMA	=39600,
	MIL_MIKE	=43200,
	MIL_NOVEMBER=-3600,
	MIL_OSCAR	=-7200,
	MIL_PAPA	=-10800,
	MIL_QUEBEC	=-14400,
	MIL_ROMEO	=-18000,
	MIL_SIERRA	=-21600,
	MIL_TANGO	=-25200,
	MIL_UNIFORM	=-28800,
	MIL_VICTOR	=-32400,
	MIL_WHISKEY	=-36000,
	MIL_XRAY	=-39600,
	MIL_YANKEE	=-43200,
	MIL_ZULU	=0,
	//United States
	US_ADT		=-10800,
	US_AKDT		=-28800,
	US_AKST		=-32400,
	US_AST		=-14400,
	US_CDT		=-18000,
	US_CST		=-21600,
	US_EDT		=-14400,
	US_EST		=-18000,
	US_HAA		=-10800,
	US_HAC		=-18000,
	US_HADT		=-32400,
	US_HAE		=-14400,
	US_HAP		=-25200,
	US_HAR		=-21600,
	US_HAST		=-36000,
	US_HAT		=-9000,
	US_HAY		=-28800,
	US_HNA		=-14400,
	US_HNC		=-21600,
	US_HNE		=-18000,
	US_HNP		=-28800,
	US_HNR		=-25200,
	US_HNT		=-12600,
	US_HNY		=-32400,
	US_HST		=-36000,
	US_MDT		=-21600,
	US_MST		=-25200,
	US_NDT		=-9000,
	US_NST		=-12600,
	US_PDT		=-25200,
	US_PST		=-28800,
};





class CalTime;

class TimeMutationWrapper
{
	friend class CalTime;
	enum TimeMember tomodify;
	CalTime *time;
	TimeMutationWrapper(enum  TimeMember m, CalTime *t) : tomodify(m), time(t){};
	public:
	CalTime operator++(int);
	CalTime operator--(int);
	CalTime operator++();
	CalTime operator--();
	CalTime operator+=( int amount );
	CalTime operator-=( int amount );

	//Time operator-(int amount);
	//Time operator+(int amount);
};






class CalTimeFormat
{
friend std::ostream & operator<<(std::ostream &, const CalTimeFormat &);
private:
  const string format_;
public:
 explicit CalTimeFormat(string format) :format_(format) {} //ctor
};









class CalTime
{
//Wrappers
	friend class TimeMutationWrapper;



private:

		day_t 		day;
		hour_t  	hour;
		minute_t	minute;
		second_t 	second;

		bool IsTimeRange;
		bool is_negative;


		//timezone_t  uctoff;
		year_t 		year;
		month_t 	month;

		//Output formats
		string Dateformat;
		string TimeRangeformat;
		int    OutputTimeZone;


public:




	bool IsLeapYear(year_t year);
	unsigned int DaysInMonth(month_t month, year_t year);



	//Creates Time object date: 1970/1/1 00:00 (Historical Unix Time (whatever that means))
	CalTime();

	//Creates a Time object with specific mode setting
	//TSM_NOW = Time is set to current system time
	//TSM_NOYEAR_MONTH = Time object does only use days, hours, minutes, seconds for calculating with times
	//					(years and months are no steady units you can not calculate with them cause some years are 366 days and months differ in count of days)
	CalTime(TimeSetupMode setupmode);

	//Creates Time object which is setup with a time_t object of <ctime>
	CalTime(time_t ctime);

	//Sets up the current time object in NoYear_Month mode
	CalTime(bool is_negative, day_t days, hour_t hours, minute_t minutes, second_t seconds);

	//Sets up the current time object to Date mode
	CalTime(year_t year,month_t month,day_t day, hour_t hour, minute_t minute, second_t second);

	//Set date with different time zone than the local one
	CalTime(TimeZone Zone, year_t year,month_t month,day_t day, hour_t hour, minute_t minute, second_t second);

    //TODO [!Function] string
	CalTime(string date);




	virtual ~CalTime();

private: //Member Functions

	CalTime Subtract_TimeRangefromDate_Elementary(CalTime *date,  CalTime *timerange);
	CalTime Add_TimeRangetoDate_Elementary(CalTime* date, CalTime* timerange);


	CalTime Add_TimeRanges_Elementary(CalTime* time1, CalTime *time2);
	CalTime Subtract_TimeRanges_Elementary(CalTime *minuend, CalTime *subtrahend);


	CalTime Add(CalTime* time1, CalTime *time2);
	CalTime Subtract(CalTime *minuend, CalTime *subtrahend);

	void INC(CalTime *time, TimeMember tomodify, unsigned int amount);
	void DEC(CalTime *time, TimeMember tomodify, unsigned int amount);

	//Prepare Outputtime for User Output in specified TimeZone
	CalTime PrepareOutputTime(TimeZone timezone);


public: //Member Functions

	//Get Local Time Zone
	int GetLocalTimeZoneSec( void );

	//Set this time object to now;
	void ToNow( void );

	//Set Time Value (for timerange mode)
	void SetTimeValue(bool is_negative, unsigned int days, unsigned int hours, unsigned int minutes, unsigned int seconds);

	//Returns true if everything is NULL
	bool IsNull();

	//Debug output of internal time on current timezone
	void PrintTimeToCout();

	//Debug output of internal time on specified timezone
	void PrintTimeToCout(TimeZone timezonedif);

	//Operators

	void operator+=(CalTime timeA)
	{
		*this=Add(this, &timeA);
	}
	void operator-=(CalTime timeA)
	{
		*this=Subtract(this, &timeA);
	}

	bool operator<(CalTime time)
	{
		int changed = 2;
		if(this->IsTimeRange==time.IsTimeRange)
		{
								    //IF both are equal -> changed=2 else changed=(this->element 	< time->element)
			if(changed == 2)changed=(this->is_negative 	!= time.is_negative	) 	? (this->is_negative	> time.is_negative	) : 2 ;
			if(changed == 2)changed=(this->year 		!= time.year		) 	? (this->year 			< time.year			) : 2 ;
			if(changed == 2)changed=(this->month		!= time.month		) 	? (this->month			< time.month		) : 2 ;
			if(changed == 2)changed=(this->day 			!= time.day			) 	? (this->day 			< time.day			) : 2 ;
			if(changed == 2)changed=(this->hour			!= time.hour		) 	? (this->hour			< time.hour			) : 2 ;
			if(changed == 2)changed=(this->minute 		!= time.minute		) 	? (this->minute 		< time.minute		) : 2 ;
			if(changed == 2)changed=(this->second		!= time.second		) 	? (this->second			< time.second		) : 2 ;
		}
		else
		{
			throw CalTimeRangeError("Objects in TimeRange mode can not be compared with objects in Date mode", LCTERR_RANGE_DATE_COMPARISON);
		}
		if(changed == 2)
		{
			return false; //That means both objects are equal so return false;
		}
		return changed;
	}

	bool operator>(CalTime time)
	{
		int changed = 2;
		if(this->IsTimeRange==time.IsTimeRange)
		{
								    //IF both are equal -> changed=2 else changed=(this->element 	< time->element)
			if(changed == 2)changed=(this->is_negative 	!= time.is_negative	) 	? (this->is_negative	< time.is_negative	) : 2 ;
			if(changed == 2)changed=(this->year 		!= time.year		) 	? (this->year 			> time.year			) : 2 ;
			if(changed == 2)changed=(this->month		!= time.month		) 	? (this->month			> time.month		) : 2 ;
			if(changed == 2)changed=(this->day 			!= time.day			) 	? (this->day 			> time.day			) : 2 ;
			if(changed == 2)changed=(this->hour			!= time.hour		) 	? (this->hour			> time.hour			) : 2 ;
			if(changed == 2)changed=(this->minute 		!= time.minute		) 	? (this->minute 		> time.minute		) : 2 ;
			if(changed == 2)changed=(this->second		!= time.second		) 	? (this->second			> time.second		) : 2 ;
		}
		else
		{
			throw CalTimeRangeError("Objects in TimeRange mode can not be compared with objects in Date mode", LCTERR_RANGE_DATE_COMPARISON);
		}
		if(changed == 2)
		{
			return false; //That means both objects are equal so return false;
		}
		return changed;
	}

	bool operator==(CalTime time)
	{
		if(!(this->IsTimeRange==time.IsTimeRange))
		{
			throw CalTimeRangeError("Objects in TimeRange mode can not be compared with objects in Date mode", LCTERR_RANGE_DATE_COMPARISON);
		}
		else if (this->IsTimeRange) //TimeRanges
		{
			return
			(
			this->is_negative 	== time.is_negative &&
			this->day 			== time.day			&&
			this->hour			== time.hour		&&
			this->minute 		== time.minute		&&
			this->second		== time.second
			);
		}
		else //Dates
		{
			return
			(
			this->year 			== time.year		&&
			this->month			== time.month		&&
			this->day 			== time.day			&&
			this->hour			== time.hour		&&
			this->minute 		== time.minute		&&
			this->second		== time.second
			);
		}

	}

	bool operator!=(CalTime time)
	{
		return !this->operator==(time);
	}

	bool operator<=(CalTime time)
	{
		return this->operator==(time) || this->operator<(time);
	}

	bool operator>=(CalTime time)
	{
		return this->operator==(time) || this->operator>(time);
	}

	friend CalTime operator+(CalTime time1, CalTime time2)
	{
		return time1.Add(&time1, &time2);
	}

	friend CalTime operator-(CalTime time1, CalTime time2)
	{
		return time1.Subtract(&time1, &time2);
	}


	void SetDateFormat(string Format)
	{
		this->Dateformat=Format;
	}
	void SetTimeRangeFormat(string Format)
	{
		this->TimeRangeformat=Format;
	}
	void SetOutputTimeZone(TimeZone zone)
	{
		if(zone < (-356459) || zone > 356459) throw CalTimeRangeError("TimeZone can not be bigger than 99:59",LCTERR_TIMEZONE_OOR);
		this->OutputTimeZone=zone;
	}



	friend ostream& operator<<(ostream& stream, const CalTime& val)
	{
		string format;
		if(val.IsTimeRange)
		{
			if(!val.TimeRangeformat.length())
			{
				format="%d days %h hours %m minutes %s seconds";
			}
			else
			{
				format=val.TimeRangeformat;
			}
		}
		else
		{
			if(!val.Dateformat.length())
			{
				format="%YYYY-%MM-%ddT%hh:%mm:%ss%S%yy:%zz";

			}
			else
			{
				format=val.Dateformat;
			}

		}

		//FORMAT
		if(val.IsTimeRange)
		{

			for(int i=0;(int)format[i];i++)
			{
				ostringstream intconv;
				if(format[i]=='%')
				{
					format.replace(i,1,""); //Each single % will just be deleted, if you want to write % you have to write %%
					switch(format[i])
					{
					case 'S': //sign
						if(val.is_negative)
						{
							format.replace(i,1,"-");
						}
						else
						{
							format.replace(i,1,"+");
						}
					break;
					case 'd':
						intconv<<val.day;
						format.replace(i,1,intconv.str());
					break;
					case 'h':
						intconv<<val.hour;
						format.replace(i,1,intconv.str());
					break;
					case 'm':
						intconv<<val.minute;
						format.replace(i,1,intconv.str());
					break;
					case 's':
						intconv<<val.second;
						format.replace(i,1,intconv.str());
					break;
					case 0: //End of String
						continue;
					case '%':
						break;
					}
				}

			}
		}
		else
		{
			//"YYYY-MM-ddThh:mm:ss+yy:zz"

			for(int i=0;(int)format[i];i++)
			{
				CalTime outtime = val;
				int timezone;
				timezone=val.OutputTimeZone;
				outtime[SECOND]+=timezone;

				ostringstream intconv;
				int maxsize,value;
				bool reduceable=0;
				if(format[i]=='%')
				{
					format.replace(i,1,""); //Each single % will just be deleted, if you want to write % you have to write %%
					char currentchar=format[i]; //We do not need to add +1 because we deleted the % so the new format[i] is the one we want already
					switch(currentchar)
					{
					case 'Y':
						value=outtime.year;
						maxsize=-1;
						reduceable=1;
					break;
					case 'M':
						value=outtime.month;
						maxsize=2;
					break;
					case 'd':
						value=outtime.day;
						maxsize=2;
					break;
					case 'h':
						value=outtime.hour;
						maxsize=2;
					break;
					case 'm':
						value=outtime.minute;
						maxsize=2;
					break;
					case 's':
						value=outtime.second;
						maxsize=2;
					break;
					case 'S': //sign for timezone
						if(val.OutputTimeZone<0)
						{
							format.replace(i,1,"-");
						}
						else
						{
							format.replace(i,1,"+");
						}
						continue;

					case 'y':
						value=abs(timezone/60/60);
						maxsize=2;
					break;
					case 'z':
						value=abs(timezone%60%60);
						maxsize=2;
					break;
					case 0: //End of String
						continue;
					case '%': //If %% leave one there and end
						continue;
					default:
						cerr<<"Deleted a lonesome % without any further action, check your Date Format String";
						continue;
					}
					unsigned int i2=i;
					unsigned int currentsize=0;
					while(format[i2]==currentchar && ((int)currentsize+1<=maxsize || maxsize == -1))//Look how long the user wants it to be
					{
						currentsize++;
						i2++;
					}
					if(reduceable && currentsize != 1)
					{
						value=value%(int)pow(10,(double)currentsize);
					}

					intconv<<setfill('0')<<setw(currentsize)<<value;
					format.replace(i,currentsize,intconv.str());
				}

			}
		}



		stream<<format;
		return stream;
	}





	class TimeMutationWrapper operator[] (enum TimeMember m){return TimeMutationWrapper(m,this);}

}; //End of Class Time







}/*End of Namespace LibTime*/

#endif /* CALTIME_HPP_ */
