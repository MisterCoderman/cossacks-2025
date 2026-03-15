#pragma pack(1)
struct Radio{
	char* xi;
	char* yi;
	word N;
};
#define RRad 90
extern Radio Rarr[RRad];

void CreateAreas();
void InitAreas();
void EraseAreas();
extern int TopLx;
extern int TopLy;
extern int MaxTop;
extern int TopSH;
extern word* TopRef;

#define SI_Center	1
#define SI_Meln		2
#define SI_Sklad	4
#define SI_Shahta	8
#define SI_Tower	16
#define SI_ArtDepo	32
#define SI_House	64
#define SI_Barrack  128
#define SI_Farm		256
#define SI_Other	512
#define SI_Near     1024
#define SI_FarLand  8192
#define SI_Okrug    16384
#define SI_Central  32768
struct StrategyInfo{
public:
	word BuildInfo;
	byte NPeasants;
	byte NShortRange;
	byte NLongRange;
	byte NMortir;
	byte NTowers;
	byte NPushek;
};
struct Area{
	short x;
	short y;
	byte  Importance;
	byte NTrees;
	byte NStones;
	//01-gold
	//02-iron
	//04-coal
	word  NMines;
	word* MinesIdx;
	word  NLinks;
	word* Link;
	word MaxLink;
	StrategyInfo SINF[8];
};
// On-disk Area layout: pointers stored as 4-byte DWORD (matches 32-bit Windows format)
struct Area_File{
	short x;
	short y;
	byte  Importance;
	byte NTrees;
	byte NStones;
	word  NMines;
	DWORD _minesIdx;
	word  NLinks;
	DWORD _link;
	word MaxLink;
	StrategyInfo SINF[8];
};
inline void AreaFromFile(Area* ar, const Area_File* af) {
	ar->x = af->x;
	ar->y = af->y;
	ar->Importance = af->Importance;
	ar->NTrees = af->NTrees;
	ar->NStones = af->NStones;
	ar->NMines = af->NMines;
	ar->MinesIdx = NULL;
	ar->NLinks = af->NLinks;
	ar->Link = NULL;
	ar->MaxLink = af->MaxLink;
	memcpy(ar->SINF, af->SINF, sizeof(ar->SINF));
}
inline void AreaToFile(Area_File* af, const Area* ar) {
	af->x = ar->x;
	af->y = ar->y;
	af->Importance = ar->Importance;
	af->NTrees = ar->NTrees;
	af->NStones = ar->NStones;
	af->NMines = ar->NMines;
	af->_minesIdx = 0;
	af->NLinks = ar->NLinks;
	af->_link = 0;
	af->MaxLink = ar->MaxLink;
	memcpy(af->SINF, ar->SINF, sizeof(af->SINF));
}
struct MediaTop{
	word* MotionLinks;
	word* LinksDist;
	word* TopRef;
	int NAreas;
	Area* TopMap;
};
extern MediaTop GTOP[2];
extern Area* TopMap;
extern int   NAreas;
extern int MaxArea;
extern word* MotionLinks;
extern word* LinksDist;
void CreateStrategyInfo();
//---------
#define WTopLx (TopLx)
#define WTopSH (TopSH)
#define WCLSH 2
#define WCLLX 4
extern word* WTopRef;
extern Area* WTopMap;
extern int   WNAreas;
extern int WMaxArea;
extern word* WMotionLinks;
extern word* WLinksDist;
//---------
void InitTopChange();
void ClearTopChange();
void AddChTop(word TopID);
void AddSurroundingAreas(word TopID);
void DynamicalLockTopCell(int x,int y);
void DynamicalUnLockTopCell(int x,int y);
void ProcessDynamicalTopology();