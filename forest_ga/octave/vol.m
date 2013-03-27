% stand, acres, vol/acre TP1, vol/acre TP2, vol/acre TP3

function YIELD = vol()

load ("-ascii", "./West_73_units_volumes.txt");

YIELD = zeros(73,4);

for iter=1:1:73
	YIELD(iter,1) = West_73_units_volumes(iter,2);
	YIELD(iter,2) = West_73_units_volumes(iter,3);
	YIELD(iter,3) = West_73_units_volumes(iter,4);
	YIELD(iter,4) = West_73_units_volumes(iter,5);
end

