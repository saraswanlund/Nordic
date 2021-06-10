#ifndef INCLUDE_EPSON_EPDC_H
#define INCLUDE_EPSON_EPDC_H 1

struct pl_epdc;
struct pl_dispinfo;
struct s1d13541;

enum epson_epdc_ref {
	EPSON_EPDC_S1D13524,
	EPSON_EPDC_S1D13541,
};

extern int epson_epdc_init(struct pl_epdc *epdc,
			   const struct pl_dispinfo *dispinfo,
			   enum epson_epdc_ref ref,
			   struct s1d13541 *s1d13541);
extern int epson_epdc_early_init(struct s1d13541 *p, enum epson_epdc_ref ref);

extern int epson_epdc_early_init_s1d13541(struct s1d13541 *p);
extern int epson_epdc_init_s1d13541(struct pl_epdc *epdc);
extern int epson_epdc_early_init_s1d13524(struct s1d13541 *p);
extern int epson_epdc_init_s1d13524(struct pl_epdc *epdc);

#endif /* INCLUDE_EPSON_EPDC_H */
