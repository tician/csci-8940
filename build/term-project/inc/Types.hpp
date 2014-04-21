/*
 *******************************************************************************
 * 
 *******************************************************************************
 * Copyright (c) 2014, Matthew Paulishen.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************
 */

#ifndef _WALKY_BOTTYPES_H_
#define _WALKY_BOTTYPES_H_

namespace Robot
{
	class RoDoFLimit
	{
	public:
		int			id;
		double		ang_min, ang_max;
		double		spd_min, spd_max;
		double		trq_min, trq_max;

		RoDoFLimit(void) :
			id(-1),
			ang_min(-180.0), ang_max(180.0),
			spd_min(-1.0), spd_max(1.0),
			trq_min(-1.0), trq_max(1.0) {};

		RoDoFLimit(	int Id,
					double amin, double amax,
					double smin, double smax,
					double tmin, double tmax) :
			id(Id),
			ang_min(amin), ang_max(amax),
			spd_min(smin), spd_max(smax),
			trq_min(tmin), trq_max(tmax) {};
	};
//	class RoDoLiClus
//	{
//	public:
//	
//	};
	
	class RoDoF
	{
	public:
		int			id;					// joint id			[-1,+inf)
		bool		en;					// joint enabled	[false,true]
		double		ang;				// joint angle		[-180.0,180.0]
		double		spd;				// joint speed		[-1.0,1.0]
		double		trq;				// joint torque		[-1.0,1.0]
		bool		inv;				// invert joint		[false,true]

		RoDoF(void) :
			id(-1), en(false), ang(0.0), spd(0.0), trq(0.0), inv(false) {};

		RoDoF(int Id, bool En, double An, double Sp, double Tr, bool In) :
			id(Id), en(En), ang(An), spd(Sp), trq(Tr), inv(In) {};
/*
		RoDoF& operator = (const RoDoF& param)
		{
			id = param.id;
			en = param.en;
			ang = param.ang;
			spd = param.spd;
			trq = param.trq;
			inv = param.inv;
			return *this;
		};
*/
	};

	class RoClus
	{
	private:
		int			num;				// number of joints in module
		RoDoF		*rho;				// pointer to array of Joints

	public:
		RoClus(void) : num(-1), rho(0) {};
		RoClus(int n) : num(-1), rho(0)
		{
			if (n > 0)
			{
				num = n;
				rho = new RoDoF[num];
			}
		};
		RoClus(RoDoF* arr, int n)
		{
			if (n > 0)
			{
				num = n;
				rho = arr;
			}
		};

		~RoClus(void) { delete[] rho; };
		void nuke(void) { ~RoClus(); };

		int size(void) { return num; };

		RoDoF& operator [] (int idx)
		{
			if (idx < num)
				return rho[idx];
			return rho[0];
		};

	};

	class dVect
	{
	public:
		int 		id;
		double		tx, ty, tz;			// translation [mm]
		double		rx, ry, rz;			// rotation	[rad]

		dVect(void) : id(-1), tx(0.0), ty(0.0), tz(0.0), rx(0.0), ry(0.0), rz(0.0) {};

		dVect(	int Id, double Tx, double Ty, double Tz,
				double Rx, double Ry, double Rz) : 
			id(Id), tx(Tx), ty(Ty), tz(Tz), rx(Rx), ry(Ry), rz(Rz) {};
	};

}

#endif

