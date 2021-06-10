#ifndef EPSON_I2C_H
#define EPSON_I2C_H 1

struct s1d13541;
struct pl_i2c;

extern int epson_i2c_init(struct s1d13541 *p, struct pl_i2c *i2c,
			  enum epson_epdc_ref ref);

#endif /* EPSON_I2C_H */