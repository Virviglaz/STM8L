/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2020 Pavel Nadein
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * STM8L101x open source driver
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "stm8l_gpio.h"

void gpio_irq(GPIO_TypeDef *gpio, enum gpio_pin pin, bool irq,
	      enum exti_edge edge)
{
	gpio->CR2 = irq ? gpio->CR2 | (u8)pin : gpio->CR2 & (~(u8)pin);

	if (!irq)
		return;

	switch (pin) {
	case PIN_0:
		EXTI->CR1 &= ~(3 << 0);
		EXTI->CR1 |= edge << 0;
		break;
	case PIN_1:
		EXTI->CR1 &= ~(3 << 2);
		EXTI->CR1 |= edge << 2;
		break;
	case PIN_2:
		EXTI->CR1 &= ~(3 << 4);
		EXTI->CR1 |= edge << 4;
		break;
	case PIN_3:
		EXTI->CR1 &= ~(3 << 6);
		EXTI->CR1 |= edge << 6;
		break;
	case PIN_4:
		EXTI->CR2 &= ~(3 << 0);
		EXTI->CR2 |= edge << 0;
		break;
	case PIN_5:
		EXTI->CR2 &= ~(3 << 2);
		EXTI->CR2 |= edge << 2;
		break;
	case PIN_6:
		EXTI->CR2 &= ~(3 << 4);
		EXTI->CR2 |= edge << 4;
		break;
	case PIN_7:
		EXTI->CR2 &= ~(3 << 6);
		EXTI->CR2 |= edge << 6;
		break;
	}
}

/* Init pin with default settings */
void gpio_init(GPIO_TypeDef *gpio, enum gpio_pin pin, enum gpio_dir dir)
{
	gpio_set_dir(gpio, pin, dir);
	if (dir == OUTPUT) {
		gpio_set_output(gpio, pin, PUSH_PULL);
		gpio_set_speed(gpio, pin, SPEED_2MHz);
		gpio_reset(gpio, pin);
	} else
		gpio_pullup(gpio, pin, false);
}