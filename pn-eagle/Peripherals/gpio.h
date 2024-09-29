#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>

typedef struct {
	volatile uint8_t *ddr;
	volatile uint8_t *port;
} gpio_t;

typedef enum {
	INPUT = 0x00,
	OUTPUT = 0x01
} gpio_mode_t;

extern const gpio_t gpio_a;
extern const gpio_t gpio_b;
extern const gpio_t gpio_c;
extern const gpio_t gpio_d;
extern const gpio_t gpio_e;
extern const gpio_t gpio_f;
extern const gpio_t gpio_g;

void gpio_pinConfig(const gpio_t *gpio, uint8_t pin, gpio_mode_t mode);
void gpio_pinSet(const gpio_t *gpio, uint8_t pin, uint8_t state);

#endif /* GPIO_H_ */