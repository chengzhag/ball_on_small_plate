#ifndef  __PID
#define  __PID

namespace greg {
	class PID {
	protected:
		float last_error;
		float last_output;
		float set_point;
		float Kp, Ki, Kd;
		float integral;
		float interval;
		float error_threshold;
		float output_upper_limit;
		float output_lower_limit;
		float error;


	public:
		/// Public constructor
		PID();

		/// Set the proportional term. Use it AFTER setRefreshInterval or setRefreshRate
		void setKp(const float &Kp);

		/// Set the integral term. Use it AFTER setRefreshInterval or setRefreshRate
		void setKi(const float &Ki);

		/// Set the derivative term. Use it AFTER setRefreshInterval or setRefreshRate
		void setKd(const float &Kd);

		/// Set the P, I, D terms respectively. Use it AFTER setRefreshInterval or setRefreshRate
		void setWeights(const float &Kp, const float &Ki, const float &Kd);

		/// Set the refresh interval of the controller in seconds.
		/// If you don't know the interval you can set the frequency of update using setRefreshRate
		void setRefreshInterval(const float &refresh_interval);

		/// Set the refresh frequency of the controller in hertz.
		/// If you don't know the refresh frequency you can set the refresh interval using setRefreshRate
		void setRefreshRate(const float &refresh_rate);

		/// Set the minimun error for computation of the PID loop. The default is 0.0
		/// It can be set to prevent integral windup or updates with minimal errors
		void setErrorThreshold(const float &error_threshold);

		/// Set the lower limit of the output. Output will be clamped to this value.
		/// If clamped, the integral will not be computed to prevent integral windup.
		/// To set the upper limit use setOutputUpperLimit
		/// YOU NEED TO SET IT!
		void setOutputLowerLimit(const float &output_lower_limit);

		/// Set the upper limit of the output. Output will be clamped to this value.
		/// If clamped, the integral will not be computed to prevent integral windup.
		/// To set the lower limit use setOutputLowerLimit
		/// YOU NEED TO SET IT!
		void setOutputUpperLimit(const float &output_upper_limit);

		/// Set the desired point of the output.
		/// This is the output the PID loop will try to reach.
		/// error will be computed subtracting the feedback input from this value
		void setDesiredPoint(const float &desired_point);

		/// The refresh funcion that make all the PID computation.
		/// Call it at specific rate, setted by setRefreshRate or setRefreshInterval (read descriptions).
		/// (use a timer interrupt if used on a embedded microcontroller)
		/// The feedback_input is the value from the feedback.
		/// The returned value is the output value of the filter.
		float refresh(const float &feedback_input);

		/// 重置PID
		void reset();
	};
};

namespace sky
{
	class PID:public greg::PID
	{
		float iSeperateThres;
		bool isInISeperateThres;
	public:
		//积分分离式PID
		//误差小于iSeperateThres时，先清零积分项，开启积分控制
		//大于时，关闭积分控制
		PID() :
			greg::PID(),
			iSeperateThres(0),
			isInISeperateThres(true)
		{

		}

		//设置积分分离式PID误差阈值
		void setISeperateThres(float iSeperateThres)
		{
			this->iSeperateThres = iSeperateThres;
		}

		float refresh(const float &feedback_input)
		{
			error = set_point - feedback_input;
			if (error >= error_threshold || error <= -error_threshold) {
				last_output = Kp*error+ Kd*(error - last_error);
				if (error > -iSeperateThres&&error < iSeperateThres)
				{
					if (!isInISeperateThres)
					{
						integral = 0;
						isInISeperateThres = true;
					}
					last_output += Ki*integral;
				}
				else
				{
					if (isInISeperateThres)
					{
						isInISeperateThres = false;
					}
				}

				if (last_output > output_upper_limit) {
					last_output = output_upper_limit;
					if (integral / error < 0.f) {
						integral += (error + last_error) / 2.f;
						last_error = error;
					}
					return output_upper_limit;
				}
				if (last_output < output_lower_limit) {
					if (integral / error < 0.f) {
						integral += (error + last_error) / 2.f;
						last_error = error;
					}
					last_output = output_lower_limit;
					return output_lower_limit;
				}
				integral += (error + last_error) / 2.f;
				last_error = error;
			}
			return last_output;
		}



	};
}

#endif
