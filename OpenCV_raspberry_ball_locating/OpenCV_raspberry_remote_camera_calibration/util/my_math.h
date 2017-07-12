#ifndef __MY_MATH_H
#define __MY_MATH_H

//限制某个数的下界
template<typename T>
void limitLow(T &num, T limL)
{
	if (num < limL)
	{
		num = limL;
	}
}

//限制某个数的上界
template<typename T>
void limitHigh(T &num, T limH)
{
	if (num > limH)
	{
		num = limH;
	}
}


//限制某个数的上下界
template<typename T>
void limit(T &num, T limL, T limH)
{
	limitLow(num, limL);
	limitHigh(num, limH);
}



#endif