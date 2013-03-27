%{
/*
 * I was really fucking bored and depressed...
 */
%}

function fitness_value = fitness(individual)

ZERO_FITNESS_LIMIT = 1.0e-5;

VERBOSE=0;

iter = 1; jter = 1;

if VERBOSE
	individual
end

xi = bitand(individual, (2^22)-1);
yi = bitshift(individual,-22);

x = (xi*0.00004768372718899898)-100.0;
y = (yi*0.00004768372718899898)-100.0;

fitness_value = 0.5 + ( ( (sin(sqrt((x^2)+(y^2))))^2 - 0.5) / (1.0+0.001*((x^2)+(y^2)))^2 );
