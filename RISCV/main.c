case 0x3: // load 0000011
			printf("Load\n");
			// TODO
			offset = imm_20_31;
			address = reg[rs1] + offset;
			int l_offset = 0;
			unsigned int word = 0;
			switch (funct3) {
			case 0b000://LB
				l_offset - address % 4;
				switch (l_offset) {
				case 0:
					int load_byte = memory[address] & 0x000000FF;
					//if the 8th bit is 1, sign extend.
					if ((load_byte & 0x100) == 0x100) {
						load_byte = (load_byte | 0xFFFFFF00); //sign extension
						reg[rd] = load_byte;
					}
					else {
						reg[rd] = load_byte;
					}
					break;
				case 1:
					int load_byte = memory[address] & 0x0000FF00;
					//if the 16th bit is 1, sign extend.
					if ((load_byte & 0x10000) == 0x10000) {
						load_byte = (load_byte | 0xFFFF0000);//sign extension
						reg[rd] = load_byte;
					}
					else {
						reg[rd] = load_byte;
					}
					break;

				case 2:
					int load_byte = memory[address] & 0x00FF0000;
					//if the 24th bit is 1, sign extend.
					if ((load_byte & 0x1000000) == 0x1000000) {
						load_byte = (load_byte | 0xFF000000);//sign extension
						reg[rd] = load_byte;
					}
					else {
						reg[rd] = load_byte;
					}
					break;
				case 3:
					int load_byte = memory[address] & 0xFF000000;
					//if the 32th bit is 1, sign extend.
					if ((load_byte & 0x100000000) == 0x100000000) {
						load_byte = (load_byte | 0x00000000);//sign extension
						reg[rd] = load_byte;
					}
					else {
						reg[rd] = load_byte;
					}
					break;

				}
			case 0b001://LH
				l_offset = address % 2;
				switch (l_offset) {

				case 0:
					int load_half = memory[address] & 0x0000FFFF;
					//if the 16th bit is 1, sign extend.
					if (load_half & 0x8000 == 0x8000) {
						load_half = load_half | 0xFFFF0000;
						reg[rd] = load_half;
					}
					else {
						reg[rd] = load_half;
					}

					break;

				case 1:
					int load_half = memory[address] & 0xFFFF0000;
					//if the last 16 bits are to be uploaded, no sign extension required.
					reg[rd] = load_half;

					break;
				}
				break;

			case 0b010://LW
				int load_word = memory[address] & 0xFFFFFFFF;
				reg[rd] = load_word;//load to rd

				break;

			case 0b100://LBU
				unsigned int load_byte = memory[address] & 0x000000FF;
				reg[rd] = load_byte;
				break;

			case 0b101://LHU
				unsigned int load_halfword = memory[address] & 0x0000FFFF;
				reg[rd] = load_halfword;
				break;
			}
			break;
