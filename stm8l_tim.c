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

#include "stm8l_tim.h"

static void (*tim2_irq_handler)(void);
static void (*tim3_irq_handler)(void);

TIM_TypeDef *tim_init(const u8 tim, u8 div, u16 period, void (*handler)(void))
{
	TIM_TypeDef *base;
	switch (tim) {
	case 2:
		CLK->PCKENR |= CLK_PCKENR_TIM2;
		tim2_irq_handler = handler;
		base = TIM2;
		break;
	case 3:
		CLK->PCKENR |= CLK_PCKENR_TIM3;
		tim3_irq_handler = handler;
		base = TIM3;
		break;
	default:
		return 0;
	}

	base->CR1 = 0;
	base->PSCR = div;
	base->CR2 = 0;
	base->ARRH = (u8)(period >> 8);
	base->ARRL = (u8)(period);
	base->EGR |= TIM_EGR_UG;
	if (handler)
		base->IER = TIM_IER_UIE;
	base->CR1 = TIM_CR1_CEN;
	return base;
}

void tim_pwm_init(TIM_TypeDef *base, const u8 ch, u16 duty)
{
	switch(ch) {
	case 1:
		base->CCER1 |= TIM_CCER1_CC1E;
		base->CCMR1 = 0x60; /* PWM1, no preload */
  		base->CCR1H = (u8)(duty >> 8);
  		base->CCR1L = (u8)(duty);
		break;
	case 2:
		base->CCER1 |= TIM_CCER1_CC2E;
		base->CCMR2 = 0x60; /* PWM1, no preload */
  		base->CCR2H = (u8)(duty >> 8);
  		base->CCR2L = (u8)(duty);
		break;
	}
}

INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_IRQHandler, 19)
{
	tim2_irq_handler();
	TIM2->SR1 = 0;
}

INTERRUPT_HANDLER(TIM3_UPD_OVF_TRG_BRK_IRQHandler, 21)
{
	tim3_irq_handler();
	TIM3->SR1 = 9;
}
