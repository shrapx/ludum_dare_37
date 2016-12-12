#ifndef TIMING_HPP_
#define TIMING_HPP_

#include <array>
#include <chrono>

using namespace std;
using namespace std::chrono;

#define FPS_SAMPLES 60

class Timing
{

private:
	typedef unsigned int uint;

  high_resolution_clock::time_point time_start;

  float time_ipo = 0.0f;
  float time_step = 1.0f / 60.0f;
  float time_last = 0.0f;
  float time_curr = 0.0f;
  float time_next = 0.0f;

  array<uint, FPS_SAMPLES> fps_samples;
  uint fps_sample_index;
	float fps = 0.0f;
	uint fps_framecount = 0;

public:

  Timing()
  {
		time_start = high_resolution_clock::now();
	};

  bool update()
  {
		duration<float> time_dur = duration_cast<duration<float>>(
			high_resolution_clock::now() - time_start);

		time_last = time_curr;
		time_curr = time_dur.count();

		if (time_curr > time_next) // update
		{

			time_next += time_step;

			fps_one_update();
			return true;
		}
		else // render
		{
			// calc render interpolation
			time_ipo = (time_curr - time_next + time_step ) / time_step;

			fps_one_render();
			return false;
		}
	};

  void fps_one_render() { fps_framecount++; }
	void fps_one_update()
  {
		if (fps_sample_index > (FPS_SAMPLES-1))
		{
			fps_sample_index = 0;
		}

		fps_samples[fps_sample_index] = fps_framecount;
		fps_framecount = 0;

		fps_sample_index++;
	}

	float get_fps() const
	{
		uint tally = 0;
		for (auto a : fps_samples )
		{
			tally += a;
		}
		return ((float)tally / (float)FPS_SAMPLES) / time_step;
	};

  void set_refresh_rate(float new_rate)
  {
		time_step = 1.0f / new_rate;
	};

	float get_change() const { return time_curr - time_last; };
  float get_current() const {return time_curr; };
	float get_step() const { return time_step; };

	/// interpolations convenience

	float get_ipo_amt() const { return time_ipo; };

	template<class T> T ipo_render(T a, T b) const
	{ return T(a + (b - a) * time_ipo); };

	template<class T> T ipo(T a, T b, float amt) const
	{ return T(a + (b - a) * amt); };

};

#endif
