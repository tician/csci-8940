

#include "Walky.hpp"

using namespace Robot;


/// partly from octave -> splines -> csape (x,y, 'periodic')
// y is the array of servo angles in the path
// numX is the number of points in path
//   y is length numX; will internally increase the length to numX+1
//   and use y_alt[numX] = y_alt[0], always (for periodicity)

// ex: spline ( 4, [1; -2; 4; 2] )
Walky::spline( int numX, double[] y)
{
	uint64_t iter=0;
	uint64_t numA = numX+1;
	uint64_t numG = numX-1;

	double *a;
	a = new double[numA];
	for (iter=0; iter<numA; iter++)
		a[iter] = y[iter];
	a[numX] = y[0];
	
	double *b, *c;
	b = new double[numA];
	c = new double[numA];

//	double *idx;
//	idx = new double[numA];

// h = ones(numX,1) (actually diff(x) with first value appended to end)
// ex: numX=4, numA=5, numG=3, a=[1;-2;4;2;1]

	// find difference between each pair of neighboring y values
//	g_i = 3*(a_i+2 - a_i+1) - 3*(a_i+1 - a_1)
	double *g; g = new double[numG];
	for (iter=0; iter<numG; iter++)
	{
		if (iter<(numG-1))
			g[iter] = 3*(a[iter+2]-a[iter+1]) - 3*(a[iter+1]-a[iter]);
		else
			g[iter] = 3*(a[1]-a[iter+1]) - 3*(a[iter+1]-a[iter]);
	}
// ex: iter=0, g[0]= 3*(4 - -2) - 3*(-2 - 1) = 18 - -9  =  27	yep
// ex: iter=1, g[1]= 3*(2 - 4)  - 3*(4 - -2) = -6 - 18  = -24	yep
// ex: iter=2, g[2]= 3*(-2 - 2) - 3*(2 - 4)  = -12 - -6 =  -6	yep

//	double *g; g = new double[numA];
//	for (iter=0; iter<numA; iter++)
//	{
//		if (iter<(numX-1))
//			g[iter] = 3*(a[iter+2]-a[iter+1]) - 3*(a[iter+1]-a[iter]);
//		else if (iter<numX)
//			g[iter] = 3*(a[0]-a[iter+1]) - 3*(a[iter+1]-a[iter]);
//		else if (iter==numX)
//			g[iter] = 3*(a[1]-a[0]) - 3*(a[iter+1]-a[iter]);
//	}

// ex: iter=0, g[0]= 3*(4 - -2) - 3*(-2 - 1) = 18 - -9 = -27	yep
// ex: iter=1, g[1]= 3*(2 - 4)  - 3*(4 - -2) = -6 - 18 = -24	yep
// ex: iter=2, g[2]= 3*(1 - 2)  - 3*(2 - 4)  = -3 - -6 = 3		nope
// ex: iter=3, g[3]= 3*(1 - 1)  - 3*(1 - 2)	meh
// ex: iter=4, g[4]= 3*(-2 - 1) - 3*(1 - 1)	meh


// ex: numX=4, numA=5, numG=3, a=[1;-2;4;2;1], g=[27;-24;-6]

	if (numX > 3)
	{
	//	dg = 2 * (h(1:n - 1) .+ h(2:n));	dg = 4*ones(numX-1,1);
	//	e = h(2:n - 1);						e  = ones(numX-2,1);
		double *dg;
		dg = new double[numG];
		for (iter=0; iter<numG; iter++)
		{
			dg[iter] = 4;
		}


// ex: dg = [4;4;4], e = [1;1];

		// Use Sherman-Morrison formula to extend the solution
		// to the cyclic system. See Numerical Recipes in C, pp 73-75
		double gamma = -4;
		dg[0] -= gamma;
// ex: dg = [8;4;4];
		dg[numG-1] -= 1/gamma;
// ex: dg = [8;4;4.25];

	//	z = spdiags( [ [e(:);0], dg, [0;e(:)] ], [-1,0,1], n-1, n-1 ) \ [ [gamma; zeros(n-3,1); h(1)], g ];
	// spdiags( [ [e(:);0], dg, [0;e(:)] ], [-1,0,1], n-1, n-1 )
	// spdiags( [ [1;1;0], [8;4;4.25], [0;1;1] ], [-1,0,1], 3, 3)
	// spdiags	(	[ 1  8   0 ]	[ -1 0 1 ], 3, 3	)	[ 8   1    0  ]
	//			(	[ 1  4   1 ]						) =	[ 1   4    1  ] 
	//			(	[ 0 4.25 1 ],						)	[ 0   1  4.25 ]
	// basically dumps dg along main diagonal, then '1' along diagonals above and below

	// [ [gamma; zeros(n-3,1); 1], g ] = [ [ -4; 0; 1], [27; -24; -6] ] =
	// [ -4   27 ]
	// [  0  -24 ]
	// [  1   -6 ]

		// create new matrices and perform solve of x=A\B => A*x = B
		// 
//		z = 


	//	fact = (z(1,2:end) + h(1) * z(end,2:end) / gamma) / (1.0 + z(1,1) + h(1) * z(end,1) / gamma);
		

		c(2:n,:) = z(:,2:end) - z(:,1) * fact;
	}

	c(1,:) = c(n,:);
	b = diff (a) ./ h(1:n - 1,idx)...
		- h(1:n - 1,idx) / 3 .* (c(2:n,:) + 2 * c(1:n - 1,:));
	b(n,:) = b(1,:);
	d = diff (c) ./ (3 * h(1:n - 1, idx));
	d(n,:) = d(1,:);


	d = d(1:n-1,:); c=c(1:n-1,:); b=b(1:n-1,:); a=a(1:n-1,:);
	pp = mkpp (x, cat (2, d'(:), c'(:), b'(:), a'(:)), szy(1:end-1));
}



