extern int Nbits;
extern cplx *state;

cplx *init_state(int bits);
cplx *resize_state(int bits, cplx *st);

void clear_state(void);
void normalize_state(void);
void measure_state(int pos, cplx *state);
void set_bit(int pos, cplx *state);
void erase_bit(int pos, cplx *state);

void print_state(void);
int  convert_label(char *str);
void print_label (int d);

void applygate(int Nbits, int ocnd, int icnd, int gpos, mat g, cplx *state);

