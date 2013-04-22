
function fitness_value = fit_var(confi, MSRT, DNVT)

% Costs
CORPS_NC	= 42;		DIV_NC		= 4;
CORPS_LEN	= 9;		DIV_LEN		= 1;
CORPS_SEN1	= 168;		DIV_SEN1	= 12;
CORPS_SEN2	= 56;		DIV_SEN2	= 4;
CORPS_SCC	= 7;		DIV_SCC		= 1;
CORPS_RAU	= 92;		DIV_RAU		= 9;
CORPS_NAI	= 4;		DIV_NAI		= 1;

NC_WIRES	= 24;
LEN_WIRES	= 176;
SEN1_WIRES	= 26;
SEN2_WIRES	= 41;

RAU_LINKS	= 25;

Percentage = 0.142857;


iter = 1; jter = 1;

% CARDINALITY TERM
for iter=1:1:config(
	sigma = (confi(
cardinality_term = 


% SENS RATIO
sen1 = (confi(CORPS_SEN1_INDEX) + confi(DIV_SEN1_INDEX));
sen2 = (confi(CORPS_SEN2_INDEX) + confi(DIV_SEN2_INDEX));

if (sen1) > (3*sen2)
	sen_ratio = (confi(CORPS_SEN1_INDEX) + confi(DIV_SEN1_INDEX)) / ...
				(confi(CORPS_SEN2_INDEX) + confi(DIV_SEN2_INDEX));
else
	sen_ratio = (confi(CORPS_SEN2_INDEX) + confi(DIV_SEN2_INDEX)) / ...
				(confi(CORPS_SEN1_INDEX) + confi(DIV_SEN1_INDEX));
end


% UHF CONNECTIVITY TERM
ant_tot = (confi(CORPS_NC_INDEX) + confi(DIV_NC_INDEX)) * 12;
ant_bac = (32 * (confi(CORPS_NC_INDEX)


fitness_value = [cardinality_term, sen_ratio, ];

