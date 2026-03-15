class ActiveZone{
public:
	int x,y,R;
	char* Name;
	byte Dir;
//--------------------//
	void Draw();
};
// On-disk ActiveZone: pointer stored as 4-byte DWORD (matches 32-bit Windows format)
struct ActiveZone_File{
	int x,y,R;
	DWORD _name;
	byte Dir;
};
inline void ActiveZoneFromFile(ActiveZone* az, const ActiveZone_File* af) {
	az->x = af->x; az->y = af->y; az->R = af->R;
	az->Name = NULL;
	az->Dir = af->Dir;
}
inline void ActiveZoneToFile(ActiveZone_File* af, const ActiveZone* az) {
	af->x = az->x; af->y = az->y; af->R = az->R;
	af->_name = 0;
	af->Dir = az->Dir;
}
class ActiveGroup{
public:
	word* Units;
	word* Serials;
	int N;
	char* Name;
	int MinX,MinY,MaxX,MaxY;
	int AminX,AminY,AmaxX,AmaxY;
	bool Visible;
	void Draw(byte c);
};
// On-disk ActiveGroup: pointers stored as 4-byte DWORD (matches 32-bit Windows format)
struct ActiveGroup_File{
	DWORD _units;
	DWORD _serials;
	int N;
	DWORD _name;
	int MinX,MinY,MaxX,MaxY;
	int AminX,AminY,AmaxX,AmaxY;
	bool Visible;
};
inline void ActiveGroupFromFile(ActiveGroup* ag, const ActiveGroup_File* af) {
	ag->Units = NULL; ag->Serials = NULL; ag->Name = NULL;
	ag->N = af->N;
	ag->MinX = af->MinX; ag->MinY = af->MinY;
	ag->MaxX = af->MaxX; ag->MaxY = af->MaxY;
	ag->AminX = af->AminX; ag->AminY = af->AminY;
	ag->AmaxX = af->AmaxX; ag->AmaxY = af->AmaxY;
	ag->Visible = af->Visible;
}
inline void ActiveGroupToFile(ActiveGroup_File* af, const ActiveGroup* ag) {
	af->_units = 0; af->_serials = 0; af->_name = 0;
	af->N = ag->N;
	af->MinX = ag->MinX; af->MinY = ag->MinY;
	af->MaxX = ag->MaxX; af->MaxY = ag->MaxY;
	af->AminX = ag->AminX; af->AminY = ag->AminY;
	af->AmaxX = ag->AmaxX; af->AmaxY = ag->AmaxY;
	af->Visible = ag->Visible;
}
extern ActiveGroup* AGroups;
extern int NAGroups;
extern int MaxAGroups;

extern ActiveZone* AZones;
extern int NAZones;
extern int MaxAZones;
void ClearAllZones();
void RefreshAGroups();