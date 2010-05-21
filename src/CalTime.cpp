/*
 * Time.cpp
 *
 *  Created on: 12.01.2010
 *      Author: cyborg-x1
 */

#include "CalTime.hpp"
#include <iomanip>
#include <stdexcept>

namespace LibCalTime {
using namespace std;


	/**
	 * Constructors//Deconstructors
	 */


	CalTime::~CalTime()
	{
		//Nothing in Here
	}


	CalTime::CalTime()
	{
		//Set this up to c historical standard date
		this->IsTimeRange=0;
		this->year=1970;
		this->month=1;
		this->day=1;
		this->hour=0;
		this->minute=0;
		this->second=0;
		this->is_negative=0;
		this->OutputTimeZone=this->GetLocalTimeZoneSec();
	}

	CalTime::CalTime(TimeSetupMode setupmode)
	{
		switch(setupmode)
		{
			case TSM_NOW:  //Set all values to current time
				this->ToNow();



			break;

			case TSM_TIME_RANGE: //Create zero noyearmonth object
				this->is_negative=false;
				this->IsTimeRange=true;
				this->second=0;
				this->minute=0;
				this->hour=0;
				this->day=0;
				this->month=0;
				this->year=0;
			break;

			default:
				throw CalTimeException("Unknown TimeSetupMode", LCTERR_UNK_TSM);
			break;
		}
		this->OutputTimeZone=this->GetLocalTimeZoneSec();
	}
	CalTime::CalTime(time_t ctime)
	{
		this->IsTimeRange=false;
		struct tm* cstruct_time;

		cstruct_time=localtime(&ctime);

		mktime(cstruct_time);

		this->second=cstruct_time->tm_sec;
		this->minute=cstruct_time->tm_min;
		this->hour=cstruct_time->tm_hour;
		this->day=cstruct_time->tm_mday;
		this->month=cstruct_time->tm_mon;
		this->year=cstruct_time->tm_year;
		//TO UCT
		this->operator[](SECOND)-=cstruct_time->tm_gmtoff;
		this->OutputTimeZone=cstruct_time->tm_gmtoff;
    }


	CalTime::CalTime(bool is_negative, day_t days, hour_t hours, minute_t minutes, second_t seconds)
	{
		this->IsTimeRange=1;
		if(seconds>=60)
		{
			minutes+=seconds/60;
			seconds=seconds%60;
		}
		if(minutes>=60)
		{
			hours+=minutes/60;
			minutes=minutes%60;
		}
		if(hours>=24)
		{
			days+=hours/24;
			hours=hours%24;
		}
		this->second=seconds;
		this->minute=minutes;
		this->hour=hours;
		this->day=days;
		this->is_negative=is_negative;

		this->year=0;
		this->month=0;
		this->OutputTimeZone=this->GetLocalTimeZoneSec();
	}

		CalTime::CalTime(year_t year,month_t month,day_t day, hour_t hour, minute_t minute, second_t second)
		{
			if(!(month>0 && month<=12)) throw CalTimeRangeError("Month value out of range (allowed range is 1-12)",LCTERR_DATEVALUE_OUT_OF_RANGE);
			if(!(day>0 && day<=this->DaysInMonth(month, year)))
				throw CalTimeException("The specified date is not possible the day does not exist or day value is 0",LCTERR_DATE_NOT_POSSIBLE);
			if(hour>23) throw CalTimeRangeError("Hour value out of range(>23)",LCTERR_DATEVALUE_OUT_OF_RANGE);
			if(minute>59) throw CalTimeRangeError("Minute value out of range(>59)",LCTERR_DATEVALUE_OUT_OF_RANGE);
			if(second>59) throw CalTimeRangeError("Second value out of range (>59)",LCTERR_DATEVALUE_OUT_OF_RANGE);

			this->year=year;
			this->month=month;
			this->day=day;
			this->hour=hour;
			this->minute=minute;
			this->second=second;

			this->is_negative=false;
			this->IsTimeRange=false;

			//TO UCT
			this->operator[](SECOND)-=GetLocalTimeZoneSec();
			this->OutputTimeZone=this->GetLocalTimeZoneSec();
		}

		CalTime::CalTime(TimeZone Zone, year_t year,month_t month,day_t day, hour_t hour, minute_t minute, second_t second)
		{
			if(!(month>0 && month<=12)) throw CalTimeRangeError("Month value out of range (allowed range is 1-12)",LCTERR_DATEVALUE_OUT_OF_RANGE);
			if(!(day>0 && day<=this->DaysInMonth(month, year)))
				throw CalTimeException("The specified date is not possible the day does not exist or day value is 0",LCTERR_DATE_NOT_POSSIBLE);
			if(hour>23)   throw CalTimeRangeError("Hour value out of range(>23)",LCTERR_DATEVALUE_OUT_OF_RANGE);
			if(minute>59) throw CalTimeRangeError("Minute value out of range(>23)",LCTERR_DATEVALUE_OUT_OF_RANGE);
			if(second>59) throw CalTimeRangeError("Second value out of range(>23)",LCTERR_DATEVALUE_OUT_OF_RANGE);

			this->year=year;
			this->month=month;
			this->day=day;
			this->hour=hour;
			this->minute=minute;
			this->second=second;

			this->is_negative=false;
			this->IsTimeRange=false;

			//TO UCT
			this->operator[](SECOND)-=Zone;
			this->OutputTimeZone=this->GetLocalTimeZoneSec();
		}

		CalTime::CalTime(string date)
		{
				bool istimerange=0;
				//Check if a timerange is given (search for d h m s)
				if( (date.find('d') != -1) || (date.find('h') != -1) || (date.find('m') != -1) || (date.find('s') != -1) ) istimerange = true;

				if(!istimerange)
				{

					int timezone_h=0;
					int timezone_m=0;
					bool timezone_given=0;
					bool timezone_neg=0;
					this->IsTimeRange=0;
					this->is_negative=0;
					this->year=0;
					this->month=1;
					this->day=1;
					this->hour=0;
					this->minute=0;
					this->second=0;

					//Select delimiters for string
					char_delimiters_separator<char> delim(true,"T+-:","");
					//Create the tokenizer
					tokenizer<> tok( date, delim);
					//Create vector to store tokens
					vector<string> datestringsplitt;
					//Store tokens in vector
					for(tokenizer<>::iterator beg=tok.begin(); beg!=tok.end();++beg)
					{
						datestringsplitt.push_back(*beg);
					}


					//Read out Date data
					for(unsigned int i=0;i<datestringsplitt.size() && i<=14;i++)
					{
						istringstream buffer(datestringsplitt[i].c_str());
						switch(i)
						{
						case 0:
								if(datestringsplitt[i].length()>_PARSE_LIMIT_YEAR_) throw CalTimeException("_PARSE_LIMIT_YEAR_ hit (define _PARSE_LIMIT_YEAR_ to exceed the maximum of 4 digits)", LCTERR_I_PARSE_ERROR);
								buffer>>this->year;
								if(buffer.fail() || !buffer.eof()) throw CalTimeException("Unallowed chars in TimeRange setup string or string is too long", LCTERR_UNALLOWED_CHAR_LENGTH);
						break;
						case 1:

								if(*datestringsplitt[i].c_str() != '-' || datestringsplitt[i].length()>1)
								{
									throw CalTimeException("Malformed date-input string! (year - month separator)", LCTERR_I_PARSE_ERROR);
								}
						break;
						case 2:
								if(datestringsplitt[i].length() > 2) throw CalTimeException("Maximum of two digits exceeded for month value",LCTERR_I_PARSE_ERROR);
								buffer>>this->month;
								if(buffer.fail() || !buffer.eof()) throw CalTimeException("Unallowed chars in TimeRange setup string or string is too long", LCTERR_UNALLOWED_CHAR_LENGTH);
								if(this->month == 0)throw CalTimeRangeError("Month value equals zero",LCTERR_I_PARSE_ERROR);

						break;
						case 3:
								if(*datestringsplitt[i].c_str() != '-'|| datestringsplitt[i].length()>1 )
								{
									throw CalTimeException("Malformed date-input string! (month - day separator)",LCTERR_I_PARSE_ERROR);
								}
						break;
						case 4:
								if(datestringsplitt[i].length() > 2) throw CalTimeException("Maximum of two digits exceeded for day value",LCTERR_I_PARSE_ERROR);
								buffer>>this->day;
								if(buffer.fail() || !buffer.eof()) throw CalTimeException("Unallowed chars in TimeRange setup string or string is too long", LCTERR_UNALLOWED_CHAR_LENGTH);
								if(this->day == 0)throw CalTimeException("MONTH EQUALS ZERO",LCTERR_I_PARSE_ERROR);
						break;
						case 5:
								if((*datestringsplitt[i].c_str() != 'T' && *datestringsplitt[i].c_str() != 't' )|| datestringsplitt[i].length()>1)
								{
									throw CalTimeException("Malformed date-input string! (date T time separator)",LCTERR_I_PARSE_ERROR);
								}
						break;
						case 6:
							    if(datestringsplitt[i].length() > 2) throw CalTimeException("Maximum of two digits exceeded for hour value",LCTERR_I_PARSE_ERROR);
								buffer>>this->hour;
								if(buffer.fail() || !buffer.eof()) throw CalTimeException("Unallowed chars in TimeRange setup string or string is too long", LCTERR_UNALLOWED_CHAR_LENGTH);
						break;
						case 7:
							if(*datestringsplitt[i].c_str() != ':' || datestringsplitt[i].length()>1 )
							{
								throw CalTimeException("Malformed date-input string! (hours:minutes separator)",LCTERR_I_PARSE_ERROR);
							}
						break;
						case 8:
							    if(datestringsplitt[i].length() > 2) throw CalTimeException("Maximum of two digits exceeded for minute value",LCTERR_I_PARSE_ERROR);
							    buffer>>this->minute;
							    if(buffer.fail() || !buffer.eof()) throw CalTimeException("Unallowed chars in TimeRange setup string or string is too long", LCTERR_UNALLOWED_CHAR_LENGTH);
						break;
						case 9:
							if(*datestringsplitt[i].c_str() != ':' || datestringsplitt[i].length()>1)
							{
								throw CalTimeException("Malformed date-input string! (minutes:second separator)",LCTERR_I_PARSE_ERROR);
							}
						break;
						case 10:
								buffer>>this->second;
								if(buffer.fail() || !buffer.eof()) throw CalTimeException("Unallowed chars in TimeRange setup string or string is too long", LCTERR_UNALLOWED_CHAR_LENGTH);
						break;
						case 11:
								if(datestringsplitt[i].length()>1) throw CalTimeException("Malformed date-input string",LCTERR_I_PARSE_ERROR);
								if(*datestringsplitt[i].c_str() == '-')
								{
									timezone_neg = 1;
								}
								else
								{
									if(*datestringsplitt[i].c_str() != '+')
									{
										throw CalTimeException("Malformed date-input string! (timezone sign)",LCTERR_I_PARSE_ERROR);
									}
								}
						break;
						case 12:
							    if(datestringsplitt[i].length() > 2) throw CalTimeException("Maximum of two digits exceeded for timezone_h value",LCTERR_I_PARSE_ERROR);
								buffer>>timezone_h;
								if(buffer.fail() || !buffer.eof()) throw CalTimeException("Unallowed chars in TimeRange setup string or string is too long", LCTERR_UNALLOWED_CHAR_LENGTH);
								timezone_given=true;

						break;
						case 13:
								if(datestringsplitt[i].length()>1) throw CalTimeException("Malformed date-input string",LCTERR_I_PARSE_ERROR);
								if(*datestringsplitt[i].c_str() != ':')
								{
									throw CalTimeException("Malformed date-input string! (timezone : seperator)",LCTERR_I_PARSE_ERROR);
								}
						break;
						case 14:
							    if(datestringsplitt[i].length() > 2) throw CalTimeException("Maximum of two digits exceeded for timezone_m value",LCTERR_I_PARSE_ERROR);
								buffer>>timezone_m;
								if(buffer.fail() || !buffer.eof()) throw CalTimeException("Unallowed chars in TimeRange setup string or string is too long", LCTERR_UNALLOWED_CHAR_LENGTH);
								if(timezone_m > 59)
								{
									throw CalTimeException("Malformed date-input string! (timezone minutes > 59)",LCTERR_I_PARSE_ERROR);
								}
						break;
						}
					}
					if(day==0 || day > this->DaysInMonth(this->month,this->year))
					{
						throw CalTimeException("The specified date is not possible the day does not exist or day value is 0",LCTERR_DATE_NOT_POSSIBLE);
					}

					if(timezone_given)
					{
						if(!timezone_neg)
						{
							this->DEC(this,MINUTE,timezone_m);
							this->DEC(this,HOUR,timezone_h);
						}
						else
						{
							this->INC(this,MINUTE,timezone_m);
							this->INC(this,HOUR,timezone_h);
						}
					}
					else
					{
						int timezone_s=this->GetLocalTimeZoneSec();
						if(timezone_s>0)
						{
							this->DEC(this,SECOND,(unsigned int)timezone_s);
						}
						else
						{
							this->INC(this,SECOND,(unsigned int)-timezone_s);
						}
					}
				}
				else //TimeRange
				{

					this->IsTimeRange=1;
					this->is_negative=0;
					this->day=0;
					this->hour=0;
					this->minute=0;
					this->second=0;

					int days=0, hours=0, minutes=0, seconds=0;

					//Select delimiters for string
					char_delimiters_separator<char> delim(true,"-dhms ","");
					//Create the tokenizer
					tokenizer<> tok( date, delim);
					//Create vector to store tokens
					vector<string> rangestringsplitt;
					//Store tokens in vector
					for(tokenizer<>::iterator beg=tok.begin(); beg!=tok.end();++beg)
					{
						rangestringsplitt.push_back(*beg);
					}

					int gotsign=0;
					int gotamount=0;

					//Read out Date data
					for(unsigned int i=0;i<rangestringsplitt.size() && i<=14;i++)
					{
						istringstream buffer(rangestringsplitt[i].c_str());



						switch(rangestringsplitt[i].length())
						{
						case 0:
							throw CalTimeException("Could be a serious error, got an empty string as tokken", LCTERR_EMPT_TOKKEN);
						break;
						case 1:
								switch(*rangestringsplitt[i].c_str())
								{
								case '+': case '-': case ' ':
									if(!gotsign)
									{
										if(gotamount) throw CalTimeException("Got Number without unit TimeRange setup string",LCTERR_TRANGE_NO_UNIT);
										if(*rangestringsplitt[i].c_str()=='+' || *rangestringsplitt[i].c_str()==' ')
										{
											gotsign=1;
										}
										else if(*rangestringsplitt[i].c_str()=='-')
										{
											gotsign=-1;
										}
										else
										{
											throw CalTimeException("Serious Error, Sign other than + or -, this should NEVER happen", LCTERR_NO_PLUSMINUS);
										}
									}
									else
									{
										throw CalTimeException("Malformed TimeRange setup string, got two signs in a row", LCTERR_TRANGE_STR);
									}
								break;

								case 'd':
								case 'h':
								case 'm':
								case 's':
									if(gotamount)
									{


										if(gotsign)gotamount*=gotsign;
										gotsign=0;

										switch(*rangestringsplitt[i].c_str())
										{
										case 'd':
											days+=gotamount;
											break;
										case 'h':
											hours+=gotamount;
											break;
										case 'm':
											minutes+=gotamount;
											break;
										case 's':
											seconds+=gotamount;
											break;
										default:
											throw CalTimeException("Serious Error, got something else than d,h,m or s, this should NEVER happen", LCTERR_TRANGE_NO_DHMS);
										}
										gotamount=0;
									}
									else
									{
										throw CalTimeException("Malformed TimeRange setup string, got unit d, h, m or s without amount",LCTERR_TRANGE_STR);
									}
								break;
								default: //to integer
									buffer>>gotamount;
									//if buffer is not on the end will mean that there are unprocessed chars in the end, fail tells that there was a non integer char in front of the string
									if(buffer.fail() || !buffer.eof()) throw CalTimeException("Unallowed chars in TimeRange setup string or string is too long", LCTERR_UNALLOWED_CHAR_LENGTH);



								}
								break;
						default://to integer
								buffer>>gotamount;
								if(buffer.fail() || !buffer.eof()) throw CalTimeException("Unallowed chars in TimeRange setup string or string is too long", LCTERR_UNALLOWED_CHAR_LENGTH);

							break;

						}
					}
					if(seconds>=0)	{	this->INC(this, SECOND,seconds); } else { this->DEC(this, SECOND,(unsigned int) seconds*(-1)); }
					if(minutes>=0)	{	this->INC(this, MINUTE,minutes); } else { this->DEC(this, MINUTE,(unsigned int) minutes*(-1)); }
					if(hours>=0)	{	this->INC(this, HOUR,hours); 	 } else { this->DEC(this, HOUR,  (unsigned int) hours  *(-1)); }
					if(days>=0)		{	this->INC(this, DAY, days );     } else { this->DEC(this, DAY,   (unsigned int) days   *(-1)); }


			}
			this->OutputTimeZone=this->GetLocalTimeZoneSec();
		}

	int CalTime::GetLocalTimeZoneSec()
	{
		time_t raw_csystem_time;
		struct tm* csystem_time;
		time ( &raw_csystem_time );
		csystem_time=localtime(&raw_csystem_time);
		mktime(csystem_time);
		return csystem_time->tm_gmtoff;
	}










	/**
	 * Implemented Functions
	 */



	//Set TimeValue
	void CalTime::SetTimeValue(bool is_negative, unsigned int days, unsigned int hours, unsigned int minutes, unsigned int seconds)
	{
		if(this->IsTimeRange)
		{
			if(seconds>=60)
			{
				minutes+=seconds/60;
				seconds=seconds%60;
			}
			if(minutes>=60)
			{
				hours+=minutes/60;
				minutes=minutes%60;
			}
			if(hours>=24)
			{
				days+=hours/24;
				hours=hours%24;
			}
			this->second=seconds;
			this->minute=minutes;
			this->hour=hours;
			this->day=days;
			this->is_negative=is_negative;
		}
		else
		{
			throw CalTimeException("WrongMode for current Time object", LCTERR_SETTIMEVALUE_WRONG_MODE);
		}
	}





	bool CalTime::IsLeapYear(year_t year)
	{
		//Read something about it somewhere in another program
		bool is_leap_year=false;
		if(year%4==0)
		{
			is_leap_year=true;
			if(year%100==0 ) //&&year>1583 ??? Does anyone need to go back so far? Hmm ... I don't think so ...
							 //according to Wikipedia this will be really too complicated here (every day betw. October 4th and 15th was left out in 1582)

			{
				is_leap_year=false;
				if(year%400==0)
				{
					is_leap_year=true;
				}
			}
		}
		return is_leap_year;
	}






	unsigned int CalTime::DaysInMonth(month_t month, year_t year)
	{
		switch(month)
		{
		case 1: case 3: case 5: case 7: case 8: case 10: case 12:
			return 31;
		break;

		case 2:
			if(IsLeapYear(year))
			{
				return 29;
			}
			else
			{
				return 28;
			}
		break;

		case 4: case 6: case 9: case 11:
			return 30;
		break;

		default:
			return ERR_MonthOutOfRange;

		}
	}






	void CalTime::ToNow( void )
	{
		time_t raw_csystem_time;
		struct tm* csystem_time;

		time ( &raw_csystem_time );

		csystem_time=localtime(&raw_csystem_time);

		mktime(csystem_time);
		this->IsTimeRange=false;
		this->second=csystem_time->tm_sec;
		this->minute=csystem_time->tm_min;
		this->hour=csystem_time->tm_hour;
		this->day=csystem_time->tm_mday;
		this->month=csystem_time->tm_mon+1;
		this->year=csystem_time->tm_year+1900; this->is_negative=false;

		//To UCT
		this->operator[](SECOND)-=csystem_time->tm_gmtoff;
	}



	void CalTime::PrintTimeToCout()
	{
		int timezonedif=this->GetLocalTimeZoneSec();
		this->PrintTimeToCout((TimeZone)timezonedif);
	}

	void CalTime::PrintTimeToCout(TimeZone timezonedif)
	{
		cout<<"Date/Time: (Time zone: "<<timezonedif<<" sec)"<<endl;


		if(this->IsTimeRange)
		{
			cout << setfill('0');
			cout<<"Time: ";
			if(this->is_negative) cout<<"-";
			cout<<setw(0)<<this->day<<"d ";
			cout<<setw(2)<<this->hour<<":";
			cout<<setw(2)<<this->minute<<":";
			cout<<setw(2)<<this->second<<endl;

			cout <<setw(0);
		}
		else
		{

			CalTime OutTime=PrepareOutputTime(timezonedif);
			cout << setfill('0');
			cout <<setw(4)<<OutTime.year<<"/"<<setw(2)<<OutTime.month<<"/"<<setw(2)<<OutTime.day<<" "<<setw(2)<<OutTime.hour<<":"<<setw(2)<<OutTime.minute<<":"<<setw(2)<<OutTime.second<<endl;
			cout <<"("<<setw(4)<<this->year<<"/"<<setw(2)<<this->month<<"/"<<setw(2)<<this->day<<" "<<setw(2)<<this->hour<<":"<<setw(2)<<this->minute<<":"<<setw(2)<<this->second<<"(internal (UTC)))"<<endl;
			cout <<setfill(' ');
			cout <<setw(0);
		}
	}







	bool CalTime::IsNull()
	{
		if(!this->IsTimeRange)throw CalTimeException("Wrong object mode for function IsNull", LCTERR_ISNULL_WRONGMODE);
		return (this->day==0 && this->hour==0 && this->minute==0 && this->second==0);
	}











	CalTime CalTime::Subtract_TimeRangefromDate_Elementary(CalTime *date,  CalTime *timerange)
	{
		CalTime result;
		result.IsTimeRange=0;
		if(!date->IsTimeRange)
		{
			//Seconds
			int mem=date->second - timerange->second;
			if(mem<0)
			{
				result.second=60 + mem;
				mem=1;
			}
			else
			{
				result.second=mem;
				mem=0;
			}
			//Minutes
			mem=date->minute - timerange->minute - mem;
			if(mem<0)
			{
				result.minute=60 + mem;
				mem=1;
			}
			else
			{
				result.minute=mem;
				mem=0;
			}
			//Hours
			mem=date->hour - timerange->hour - mem;
			if(mem<0)
			{
				result.hour=24 + mem;
				mem=1;
			}
			else
			{
				result.hour=mem;
				mem=0;
			}
			//Days
			result.month=date->month;
			result.year=date->year;
			result.day=date->day;
			mem=result.day - timerange->day - mem;
			while(mem<=0)
			{
				result.month--;
				if(result.month <= 0) //If month <= 0  subtract year
				{
					if(result.year!=0)
					{
						result.year--;
					}
					else
					{
						throw CalTimeRangeError("Negative Years are not supported by LibTime!", LCTERR_NEG_YEAR);
					}
					result.month=12;
				}

				mem=this->DaysInMonth(result.month,result.year) + mem;
			}
			result.day=mem;

		}
		else
		{
			throw CalTimeException("Expected object in Date mode, got one in TimeRange mode",LCTERR_ADDSUB_TRD_WRONGMODE);
		}
		return result;
	}








	CalTime CalTime::Add_TimeRangetoDate_Elementary(CalTime* date, CalTime* timerange)
	{
		CalTime result;
		result.IsTimeRange=0;
		if(timerange->IsTimeRange)
		{
			unsigned int mem=date->second+timerange->second;
			result.second=mem%60;

			mem=mem/60+date->minute+timerange->minute;
			result.minute=mem%60;


			mem=mem/60+date->hour+timerange->hour;
			result.hour=mem%24;


			mem=mem/24+timerange->day;

			result.day=date->day;
			result.month=date->month;
			result.year=date->year;

			while(mem!=0)
			{
				if(DaysInMonth(result.month, result.year)-result.day +1   <=      mem)
				{
					mem-=(DaysInMonth(result.month, result.year)-result.day+1);
					result.day=1;
					if(result.month + 1 > 12)
					{
						result.year++;
						result.month=1;
					}
					else
					{
						result.month++;
					}

				}
				else
				{
					result.day+=mem;
					mem=0;
				}

			}
		}
		else
		{
			throw CalTimeException("Expected object in Date mode, got one in TimeRange mode",LCTERR_ADDSUB_TRD_WRONGMODE);
		}
		return result;
	}








	CalTime CalTime::Add_TimeRanges_Elementary(CalTime *time1, CalTime *time2)
	{
		CalTime result(TSM_TIME_RANGE);
		if(this->IsTimeRange)
		{
			unsigned int mem=time1->second+time2->second;
			result.second=mem%60;

			mem=mem/60+this->minute+time2->minute;
			result.minute=mem%60;


			mem=mem/60+this->hour+time2->hour;
			result.hour=mem%24;


			result.day=mem/24+time2->day+time1->day;
		}
		else
		{
			throw CalTimeException("Called function used to add objects in TimeRange mode but got a object in Date mode", LCTERR_TRADDSUBELEM_WRONGMODE);
		}
		return result;
	}








	CalTime CalTime::Subtract_TimeRanges_Elementary( CalTime *minuend, CalTime *subtrahend)
	{
		CalTime result(TSM_TIME_RANGE);
		CalTime *dummyptr;
		if(minuend->IsTimeRange && subtrahend->IsTimeRange) //Both Elements are NoYearMonth
		{
		/**This function subtracts a value (timeB) from timeA ... and changes the sign...
		 * !!! BUT IT DOES NOT CARE OF THE SIGNS BEFORE !!!
		 * !!! THIS FUNCTION IS CALLED FROM ANOTHER FUNCTION WHICH DOES THE SIGN STUFF
		 */
		unsigned int changed=2;

		//Check which Element is bigger(absolute)
		if(changed == 2)changed=(minuend->day 	!= subtrahend->day		) 	? (minuend->day 	< subtrahend->day		) : 2 ;
		if(changed == 2)changed=(minuend->hour 	!= subtrahend->hour		) 	? (minuend->hour 	< subtrahend->hour		) : 2 ;
		if(changed == 2)changed=(minuend->minute!= subtrahend->minute	) 	? (minuend->minute	< subtrahend->minute	) : 2 ;
		if(changed == 2)changed=(minuend->second!= subtrahend->second	) 	? (minuend->second	< subtrahend->second	) : 2 ;

		//If the subtrahend is bigger than the minuend then the result will pass 0...
		//To implement that easily we exchange minuend and subtrahend with each other and negate the sign
		// c=a-b => -c=-a+b => -c=b-a
		if(changed==1)
		{
			//minuend is the smaller so the sign of result is the negated of the minuend
			result.is_negative=!minuend->is_negative;
			//exchange minuend and subtrahend ... so that we subtract the bigger from the smaller
			dummyptr=minuend;
			minuend=subtrahend;
			subtrahend=dummyptr;

		}
		else
		{
			//If minuend is bigger subtrahend result gets its sign
			result.is_negative=minuend->is_negative;
		}

		//Calculate Result

			//Seconds
			int mem=minuend->second - subtrahend->second;
			if(mem<0)
			{
				result.second=60 + mem;
				mem=1;
			}
			else
			{
				result.second=mem;
				mem=0;
			}
			//Minutes
			mem=minuend->minute - subtrahend->minute - mem;
			if(mem<0)
			{
				result.minute=60 + mem;
				mem=1;
			}
			else
			{
				result.minute=mem;
				mem=0;
			}
			//Hours
			mem=minuend->hour - subtrahend->hour - mem;
			if(mem<0)
			{
				result.hour=24 + mem;
				mem=1;
			}
			else
			{
				result.hour=mem;
				mem=0;
			}
			//Days
			mem=minuend->day - subtrahend->day - mem;
			if(mem<0)
			{
				mem*=(-1);
			}
			result.day=mem;
		}
		else
		{
			throw CalTimeException("Called function used to subtract objects in TimeRange mode but got a object in Date mode", LCTERR_TRADDSUBELEM_WRONGMODE);
		}
		return result;
	}







	CalTime CalTime::Add(CalTime* time1, CalTime *time2)
	{
		CalTime result;
		if(!(!time1->IsTimeRange && !time2->IsTimeRange)) //Two dates can not be added
		{
			if(time1->IsTimeRange && time2->IsTimeRange)
			{
				result=CalTime(TSM_TIME_RANGE);
				if     (time1->is_negative == time2->is_negative) result=     Add_TimeRanges_Elementary(time1, time2);//If both signs are even call add function
				else if(time1->is_negative >  time2->is_negative) result=Subtract_TimeRanges_Elementary(time2, time1);//If different call subtract function
				else if(time1->is_negative <  time2->is_negative) result=Subtract_TimeRanges_Elementary(time1, time2);//the negative must be the second variable
			}
			else
			{
				CalTime *dateptr;
				CalTime *timeptr;

				if(time1->IsTimeRange && !time2->IsTimeRange)
				{
					dateptr=time2;
					timeptr=time1;
				}
				else if(!time1->IsTimeRange && time2->IsTimeRange)
				{
					dateptr=time1;
					timeptr=time2;
				}
				if(timeptr->is_negative)
				{
					result=Subtract_TimeRangefromDate_Elementary(dateptr, timeptr);
				}
				else
				{
					result=Add_TimeRangetoDate_Elementary(dateptr, timeptr);
				}

			}
		}
		else
		{
			//Dates can not be added because of the non consistent months(31, 30 or 29 days) or years (365 or 366 days)
			throw CalTimeException("Tried to add two objects in date mode",LCTERR_DATES_ADD);
		}
		return result;
	}








	CalTime CalTime::Subtract(CalTime *minuend, CalTime *subtrahend)
	{
		CalTime result;
		if(minuend->IsTimeRange!=0 || subtrahend->IsTimeRange!=0) //Difference between two dates ->
		{

			if(minuend->IsTimeRange && subtrahend->IsTimeRange)
			{
				if(minuend->is_negative == subtrahend->is_negative) result=Subtract_TimeRanges_Elementary(minuend, subtrahend);//If both signs are even call add function
				else if (minuend->is_negative >  subtrahend->is_negative)
				{
					 result=Add_TimeRanges_Elementary(minuend, subtrahend);
					 result.is_negative=minuend->is_negative;
				}
				else if (minuend->is_negative <  subtrahend->is_negative)
				{
					result=Add_TimeRanges_Elementary(minuend, subtrahend);
					result.is_negative=minuend->is_negative;
				}
			}
			else  //Mixed (Dates and TimeRange)
			{
				CalTime *dateptr;
				CalTime *timeptr;

				if(!minuend->IsTimeRange && subtrahend->IsTimeRange)
				{
					dateptr=minuend;
					timeptr=subtrahend;
				}
				else if(minuend->IsTimeRange && !subtrahend->IsTimeRange)
				{
					dateptr=subtrahend;
					timeptr=minuend;
				}
				if(timeptr->is_negative)
				{
					result=Add_TimeRangetoDate_Elementary(dateptr, timeptr);
				}
				else
				{
					result=Subtract_TimeRangefromDate_Elementary(dateptr, timeptr);
				}
			}
		}
		else
		{
			//Calculate Difference between two Dates

			if(*subtrahend>*minuend)
			{
				CalTime *memptr;
				memptr=minuend;
				minuend=subtrahend;
				subtrahend=memptr;
				result.is_negative=1;
			}

			result.IsTimeRange=1;
			//Seconds
			int mem=minuend->second - subtrahend->second;
			if(mem<0)
			{
				result.second=60 + mem;
				mem=1;
			}
			else
			{
				result.second=mem;
				mem=0;
			}
			//Minutes
			mem=minuend->minute - subtrahend->minute - mem;
			if(mem<0)
			{
				result.minute=60 + mem;
				mem=1;
			}
			else
			{
				result.minute=mem;
				mem=0;
			}
			//Hours
			mem=minuend->hour - subtrahend->hour - mem;
			if(mem<0)
			{
				result.hour=24 + mem;
				mem=1;
			}
			else
			{
				result.hour=mem;
				mem=0;
			}
			//Days


			unsigned int yearM, yearS, monthM, monthS;
			int dayM,dayS;

			result.day=0;

			yearM=minuend->year;
			yearS=subtrahend->year;
			monthM=minuend->month;
			monthS=subtrahend->month;
			dayM=minuend->day;
			dayS=subtrahend->day;




			while(yearM!=yearS || monthM != monthS || dayM != dayS)
			{
				dayM--;
				if(dayM==0)
				{
					monthM--;
					if(monthM==0)
					{
						monthM=12;
						yearM--;
					}
					dayM=DaysInMonth(monthM,yearM);
				}
				result.day++;
			}
			result.day-=mem;






		}
		return result;
	}


	void CalTime::INC(CalTime *time, TimeMember tomodify, unsigned int amount)
	{
		bool normadd=0;
		unsigned int days=0,hours=0,minutes=0,seconds=0;
		switch(tomodify)
		{

		case YEAR:
			if(!time->IsTimeRange)
			{
				time->year+=amount;
				days=time->DaysInMonth(time->month,time->year);
				if(days<time->day)
				{
					time->day=days;
				}
			}
			else
			{
				throw CalTimeException("Years can not be increased in a time range object", LCTERR_INCDEC_WRONGMODE);
			}
		break;

		case MONTH:
			for(unsigned int i=0; i<amount; i++)
			{
				if(!time->IsTimeRange)
				{
					time->month++;
					if(time->month>12)
					{
						time->month=1;
						time->year++;
					}
					days=time->DaysInMonth(time->month,time->year);
					if(days<time->day)
					{
						time->day=days;
					}
				}
				else
				{
					throw CalTimeException("Months can not be increased in a time range object", LCTERR_INCDEC_WRONGMODE);
				}
			}
		break;

		case DAY:
			normadd=1;
			days=amount;
		break;
		case HOUR:
			normadd=1;
			hours=amount;
		break;
		case MINUTE:
			normadd=1;
			minutes=amount;
		break;
		case SECOND:
			normadd=1;
			seconds=amount;
		break;
		}
		CalTime tobeadded(false,days,hours,minutes,seconds);
		if(normadd)
		{
			*time+=tobeadded;
		}
	}

	void CalTime::DEC(CalTime *time, TimeMember tomodify, unsigned int amount)
	{
		bool normadd=0;
		unsigned int days=0,hours=0,minutes=0,seconds=0;
		switch(tomodify)
		{

		case YEAR:
			if(!time->IsTimeRange)
			{
				if((time->year-amount)<0)
				{
					throw CalTimeRangeError("Negative Years are not supported!", LCTERR_NEG_YEAR);
				}
				else
				{
					time->year-=amount;
					days=time->DaysInMonth(time->month,time->year);
					if(days<time->day)
					{
						time->day=days;
					}
				}
			}
			else
			{
				throw CalTimeException("Years can not be decreased in a time range object", LCTERR_INCDEC_WRONGMODE);
			}
		break;

		case MONTH:
			for(unsigned int i=0; i<amount; i++)
			{
				if(!time->IsTimeRange)
				{
					time->month--;
					if(time->month==0)
					{
						if((time->year-1)<0)
						{
							throw CalTimeRangeError("Negative Years are not supported!", LCTERR_NEG_YEAR);
						}
						else
						{
							time->month=12;
							time->year--;
						}
					}
					days=time->DaysInMonth(time->month,time->year);
					if(days<time->day)
					{
						time->day=days;
					}
				}
				else
				{
					throw CalTimeException("Months can not be increased in a time range object", LCTERR_INCDEC_WRONGMODE);
				}
			}
		break;

		//Parts that can be added normally (not just up counting like those above)
		case DAY:
			normadd=1;
			days=amount;
		break;
		case HOUR:
			normadd=1;
			hours=amount;
		break;
		case MINUTE:
			normadd=1;
			minutes=amount;
		break;
		case SECOND:
			normadd=1;
			seconds=amount;
		break;
		}
		CalTime tobeadded(false,days,hours,minutes,seconds);
		if(normadd)
		{
			*time-=tobeadded;
		}
	}















	/*
	 *
	 *
	 *
	 * Functions of TimeMutationWrapper
	 *
	 *
	 *
	 */

	CalTime TimeMutationWrapper::operator++(int)
	{
		this->time->INC(this->time,this->tomodify,1);
		return *this->time;
	}

	CalTime TimeMutationWrapper::operator--(int)
	{
		this->time->DEC(this->time,this->tomodify,1);
		return *this->time;
	}


	CalTime TimeMutationWrapper::operator++()
	{
		this->time->INC(this->time,this->tomodify,1);
		return *this->time;
	}


	CalTime TimeMutationWrapper::operator--()
	{
		this->time->DEC(this->time,this->tomodify,1);
		return *this->time;
	}

	CalTime TimeMutationWrapper::operator+=( int amount )
	{
		if(amount>=0)
		{
			this->time->INC(this->time,this->tomodify,amount);
		}
		else
		{
			this->operator-=(-amount);
		}
		return *this->time;
	}

	CalTime TimeMutationWrapper::operator-=( int amount )
	{
		if(amount>=0)
		{
			this->time->DEC(this->time,this->tomodify,amount);
		}
		else
		{
			this->operator+=(-amount);
		}
		return *this->time;
	}

	CalTime CalTime::PrepareOutputTime(TimeZone timezone)
	{
		CalTime outputtime=*this;
		if(!this->IsTimeRange)
		{

				outputtime[SECOND]+=(int) timezone;
		}
		else
		{
			throw CalTimeException("PrepareOutputTime called for object in TimeRange mode",LCTERR_PRPOUTTIME_MODE);
		}
		return outputtime;
	}




}//Namespace end
