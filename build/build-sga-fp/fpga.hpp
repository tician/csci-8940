
typedef struct
{
	uint64_t	id;
	double		area;
	uint64_t	y1;
	uint64_t	y2;
	uint64_t	y3;
} fp_volume_t;

fp_volume_t West73_Yields[] =
{
	{1,5.69,29,43,55},
	{2,28.128,60,69,78},
	{3,29.941,60,69,78},
	{4,17.068,8,17,29},
	{5,37.208,3,8,17},
	{6,41.896,3,8,17},
	{7,19.977,43,55,65},
	{8,17.68,0,3,8},
	{9,9.049,0,3,8},
	{10,12.928,92,92,92},
	{11,60.687,92,92,92},
	{12,63.081,55,65,74},
	{13,19.648,0,5,12},
	{14,55.53,17,29,43},
	{15,63.084,23,36,49},
	{16,5.604,23,36,49},
	{17,2.896,23,36,49},
	{18,21.42,0,3,8},
	{19,35.477,0,3,8},
	{20,23.226,0,3,8},
	{21,11.43,0,5,12},
	{22,22.526,0,5,12},
	{23,7.38,29,43,55},
	{24,19.599,23,36,49},
	{25,2.932,23,36,49},
	{26,41.835,36,49,60},
	{27,45.981,29,43,55},
	{28,.696,69,78,84},
	{29,11.996,69,78,84},
	{30,69.151,17,29,43},
	{31,66.031,17,29,43},
	{32,33.174,17,29,43},
	{33,50.021,43,55,65},
	{34,85.919,29,43,55},
	{35,55.73,29,43,55},
	{36,.85,43,55,65},
	{37,15.047,65,74,81},
	{38,13.989,65,74,81},
	{39,52.743,65,74,81},
	{40,3.883,0,5,12},
	{41,60.805,0,5,12},
	{42,29.699,3,8,17},
	{43,16.726,3,8,17},
	{44,34.11,92,92,92},
	{45,14.699,92,92,92},
	{46,46.272,92,92,92},
	{47,19.541,49,60,69},
	{48,62.531,49,60,69},
	{49,39.273,92,92,92},
	{50,47.416,49,60,69},
	{51,34.353,3,8,17},
	{52,3.165,3,8,17},
	{53,41.985,60,69,78},
	{54,37.832,3,8,17},
	{55,44.699,29,43,55},
	{56,39.582,29,43,55},
	{57,67.783,60,69,78},
	{58,4.285,60,69,78},
	{59,27.88,3,8,17},
	{60,39.044,43,55,65},
	{61,48.645,43,55,65},
	{62,19.469,43,55,65},
	{63,1.908,43,55,65},
	{64,.633,12,23,36},
	{65,41.532,12,23,36},
	{66,45.98,8,17,29},
	{67,82.437,8,17,29},
	{68,16.315,23,36,49},
	{69,48.27,23,36,49},
	{70,.315,49,60,69},
	{71,3.211,49,60,69},
	{72,8.533,49,60,69},
	{73,5.826,49,60,69}
};

typedef struct
{
	uint64_t	id;
	GENO_TYPE	mask;
} fp_adjacency_t;

fp_adjacency_t West73_Adjacency[] =
{
	{1,		~( (1<<(6-1)) | (1<<(34-1)) | (1<<(48-1)) ) },
	{2,		~( (1<<(3-1)) | (1<<(66-1)) | (1<<(67-1)) ) },
	{3,		~( (1<<(2-1)) | (1<<(4-1)) ) },
	{4,		~( (1<<(3-1)) | (1<<(67-1)) ) },
	{5,		~( (1<<(6-1)) | (1<<(12-1)) | (1<<(15-1)) | (1<<(27-1)) | (1<<(34-1)) ) },
	{6,		~( (1<<(1-1)) | (1<<(5-1)) | (1<<(17-1)) | (1<<(48-1)) | (1<<(50-1)) ) },
	{7,		~( (1<<(67-1)) | (1<<(72-1)) ) },
	{8,		~( (1<<(9-1)) | (1<<(69-1)) | (1<<(71-1)) ) },
	{9,		~( (1<<(8-1)) ) },
	{10,	~( (1<<(11-1)) | (1<<(49-1)) ) },
	{11,	~( (1<<(10-1)) | (1<<(13-1)) | (1<<(19-1)) | (1<<(49-1)) | (1<<(53-1)) ) },
	{12,	~( (1<<(5-1)) | (1<<(18-1)) | (1<<(27-1)) ) },
	{13,	~( (1<<(11-1)) | (1<<(51-1)) ) },
	{14,	~( (1<<(15-1)) | (1<<(18-1)) | (1<<(30-1)) | (1<<(31-1)) | (1<<(32-1)) | (1<<(45-1)) | (1<<(46-1)) ) },
	{15,	~( (1<<(5-1)) | (1<<(14-1)) | (1<<(16-1)) | (1<<(17-1)) | (1<<(18-1)) | (1<<(20-1)) | (1<<(31-1)) | (1<<(50-1)) | (1<<(55-1)) ) },
	{16,	~( (1<<(15-1)) | (1<<(20-1)) | (1<<(49-1)) ) },
	{17,	~( (1<<(6-1)) | (1<<(15-1)) | (1<<(50-1)) ) },
	{18,	~( (1<<(12-1)) | (1<<(14-1)) | (1<<(15-1)) ) },
	{19,	~( (1<<(11-1)) | (1<<(20-1)) | (1<<(49-1)) | (1<<(54-1)) ) },
	{20,	~( (1<<(15-1)) | (1<<(16-1)) | (1<<(19-1)) | (1<<(54-1)) ) },
	{21,	~( (1<<(22-1)) | (1<<(24-1)) ) },
	{22,	~( (1<<(21-1)) | (1<<(25-1)) ) },
	{23,	~( (1<<(31-1)) | (1<<(41-1)) | (1<<(55-1)) ) },
	{24,	~( (1<<(21-1)) | (1<<(25-1)) | (1<<(61-1)) ) },
	{25,	~( (1<<(22-1)) | (1<<(24-1)) | (1<<(63-1)) ) },
	{26,	~( (1<<(39-1)) | (1<<(42-1)) | (1<<(44-1)) ) },
	{27,	~( (1<<(5-1)) | (1<<(12-1)) | (1<<(28-1)) ) },
	{28,	~( (1<<(27-1)) ) },
	{29,	~( (1<<(34-1)) | (1<<(35-1)) ) },
	{30,	~( (1<<(14-1)) | (1<<(37-1)) | (1<<(45-1)) ) },
	{31,	~( (1<<(14-1)) | (1<<(15-1)) | (1<<(23-1)) | (1<<(38-1)) | (1<<(41-1)) | (1<<(43-1)) | (1<<(55-1)) ) },
	{32,	~( (1<<(14-1)) ) },
	{33,	~( (1<<(34-1)) ) },
	{34,	~( (1<<(1-1)) | (1<<(5-1)) | (1<<(29-1)) | (1<<(33-1)) | (1<<(47-1)) | (1<<(48-1)) ) },
	{35,	~( (1<<(29-1)) | (1<<(36-1)) ) },
	{36,	~( (1<<(35-1)) ) },
	{37,	~( (1<<(30-1)) ) },
	{38,	~( (1<<(31-1)) | (1<<(43-1)) ) },
	{39,	~( (1<<(26-1)) ) },
	{40,	~( (1<<(42-1)) ) },
	{41,	~( (1<<(23-1)) | (1<<(31-1)) | (1<<(43-1)) | (1<<(55-1)) | (1<<(56-1)) ) },
	{42,	~( (1<<(26-1)) | (1<<(40-1)) | (1<<(44-1)) ) },
	{43,	~( (1<<(31-1)) | (1<<(38-1)) | (1<<(41-1)) ) },
	{44,	~( (1<<(26-1)) | (1<<(42-1)) ) },
	{45,	~( (1<<(14-1)) | (1<<(30-1)) ) },
	{46,	~( (1<<(14-1)) ) },
	{47,	~( (1<<(34-1)) ) },
	{48,	~( (1<<(1-1)) | (1<<(6-1)) | (1<<(34-1)) ) },
	{49,	~( (1<<(10-1)) | (1<<(11-1)) | (1<<(16-1)) | (1<<(19-1)) ) },
	{50,	~( (1<<(6-1)) | (1<<(15-1)) | (1<<(17-1)) ) },
	{51,	~( (1<<(13-1)) | (1<<(53-1)) ) },
	{52,	~( (1<<(57-1)) ) },
	{53,	~( (1<<(11-1)) | (1<<(51-1)) | (1<<(54-1)) ) },
	{54,	~( (1<<(19-1)) | (1<<(20-1)) | (1<<(53-1)) ) },
	{55,	~( (1<<(15-1)) | (1<<(23-1)) | (1<<(31-1)) | (1<<(41-1)) | (1<<(59-1)) ) },
	{56,	~( (1<<(41-1)) | (1<<(58-1)) | (1<<(60-1)) | (1<<(62-1)) ) },
	{57,	~( (1<<(52-1)) | (1<<(59-1)) ) },
	{58,	~( (1<<(56-1)) ) },
	{59,	~( (1<<(55-1)) | (1<<(57-1)) ) },
	{60,	~( (1<<(56-1)) | (1<<(62-1)) | (1<<(64-1)) ) },
	{61,	~( (1<<(24-1)) | (1<<(63-1)) | (1<<(65-1)) ) },
	{62,	~( (1<<(56-1)) | (1<<(60-1)) ) },
	{63,	~( (1<<(25-1)) | (1<<(61-1)) ) },
	{64,	~( (1<<(60-1)) ) },
	{65,	~( (1<<(61-1)) ) },
	{66,	~( (1<<(2-1)) | (1<<(73-1)) ) },
	{67,	~( (1<<(2-1)) | (1<<(4-1)) | (1<<(7-1)) | (1<<(72-1)) ) },
	{68,	~( (1<<(70-1)) ) },
	{69,	~( (1<<(8-1)) | (1<<(71-1)) ) },
	{70,	~( (1<<(68-1)) ) },
	{71,	~( (1<<(8-1)) | (1<<(69-1)) ) },
	{72,	~( (1<<(7-1)) | (1<<(67-1)) ) },
	{73,	~( (1<<(66-1)) ) }
};