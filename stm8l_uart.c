/*
 * This file is provided under a MIT license.  When using or
 *   redistributing this file, you may do so under either license.
 *
 *   MIT License
 *
 *   Copyright (c) 2019 Pavel Nadein
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * STM8L101x open source driver
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "stm8l_uart.h"
#include "stm8l_clk.h"

static struct
{
	volatile char *rx_buf;
	volatile uint8_t pos;
	uint8_t size;
	bool is_done;
} rx;

void uart_init(uint16_t freq)
{
	uint16_t uart_div = clk_get_freq_MHz() * 1000000u / freq;
	CLK->PCKENR |= CLK_PCKENR_USART;

	USART->BRR1 = (uart_div & 0x0FF0) >> 4;
	USART->BRR2 = (uart_div & 0xF000) >> 8 | (uart_div & 0x000F);
	USART->CR2 = USART_CR2_TEN;
}

void uart_enable_rx_irq(char *buf, uint16_t size)
{
	rx.rx_buf = buf;
	rx.size = size;
	rx.pos = 0;
	rx.is_done = false;

	USART->CR2 |= USART_CR2_RIEN | USART_CR2_REN;
}

uint16_t uart_check_rx(void)
{
	u8 len = rx.pos;
	if (!rx.is_done)
		return 0;

	rx.pos = 0;
	rx.is_done = false;

	return len;
}

void uart_send_string(const char *buf)
{
	while(*buf) {
		while(!(USART->SR & USART_SR_TXE));
		USART->DR = *buf++;
	}
}

INTERRUPT_HANDLER(USART_RX_IRQHandler, 28)
{
	char data = USART->DR;

	/* If buffer is still full, do nothing */
	if (rx.pos == rx.size || rx.is_done)
		return;

	/* Buffer full or newline received */
	if (data == '\n' || data == '\r' || data == 0) {
		rx.rx_buf[rx.pos] = 0; /* null terminate */
		rx.is_done = true;
		return;
	}

	rx.rx_buf[rx.pos++] = data;
}