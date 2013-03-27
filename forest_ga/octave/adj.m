
function ADJ_MSK = adj()

load ("-ascii", "./West_73_units_adjacency.txt");
[nr,nc] = size(West_73_units_adjacency);

ADJ_MSK = zeros(West_73_units_adjacency(nr,1),West_73_units_adjacency(nr,1));

for iter=1:1:nr
	ADJ_MSK(West_73_units_adjacency(iter,1),West_73_units_adjacency(iter,2)) = 1;
end

