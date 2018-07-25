//////////////////////////////////////////////////////////////////////////
//                                                                      //
//            ������� ����������� ����������� ���������������           //
//                                                                      //
//       ���������� ������� � ���������� ���������� ������� DWARF       //
//                                                                      //
//               �������������� ������ pro_glin.cpp                     //
//                                                                      //
//  ���� ���������  ������� Dwarf_P_Debug::generate_debugline           //
//                                                                      //
//                                                                                         //
//  If this code works, it was                                                         //
//  If not, I don't know who wrote it.                                  //
//                                                                      //
// $Revision:: 1     $      $Date:: 7/21/05 5:33p    $                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------//
//          ������� ������ Dwarf_P_Debug                            //
//          generate_debugline()                                    //
//          ������� ������ .debug_line                              //
//------------------------------------------------------------------//

#include <string.h>

#include "libelf.h"
#include "dwarferr.h"
#include "dwarftyp.h"
#include "dwarf.h"
#include "libdwarf.h"
#include "dwarfint.h"

#include "pro_line.h"
#include "pro_dbg.h"

Dwarf_Signed Dwarf_P_Debug::generate_debugline(Dwarf_P_CU *cu) throw(Dwarf_Error)
{
        // ����������, � ������� ������������� � cur, ������������
        // ��� ������ �� ���������� ��������
    Dwarf_P_Inc_Dir *curdir;
    Dwarf_P_F_Entry *curentry;
    Dwarf_P_Line *curline;
    Dwarf_P_Line *prevline;

        // ��������� �� ������ � �� ����� ������ ��������� ������� 
        // �����������
    Dwarf_P_Rel *rel_head, *rel_tail;

    int rel_nbytes;     // ����� ������ � ������� �����������
    int sum_bytes;
    int prolog_size;
    unsigned char *data;    // ������ ������ � �������� �����
    int elfsectno;
    Dwarf_Unsigned name_idx;    // ������ ����� ������ � ������� SYMTAB
    unsigned char *start_sec;   // ��������� �� ����� ������ ������
                                // � ����� ������ ������

        // ��������� ���������� ��� ������ ������
    Dwarf_Unsigned du;
    Dwarf_Word dw;
    Dwarf_Ubyte db;
    Dwarf_Half dh;
    char buff1[ENCODE_SPACE_NEEDED];


    if ( cu == NULL ) return dpd_n_debug_sect;
    if (cu->dpc_lines == NULL) return dpd_n_debug_sect;
    sum_bytes = 0;
    rel_nbytes = 0;
    rel_head = NULL;
    rel_tail = NULL;

    cu->dpc_line_offset = dpd_next_line_offset;

    elfsectno = Dwarf::elf_sects[DEBUG_LINE];

	    // �������� ������ ���������� ���������
    prolog_size = 0;

        // ���������� ��������
    curdir = cu->dpc_inc_dirs;

    while (curdir) {
        prolog_size += strlen(curdir->dpid_name)+1;
        curdir = curdir->next;
    }
    prolog_size++;  // ��������� ������� ���� ����� ���������� ��������

        // ����� ������
    curentry = cu->dpc_file_entries;

    while (curentry) {
        prolog_size += strlen(curentry->dpfe_name)+1+curentry->dpfe_nbytes;
        curentry = curentry->next;
    }

    prolog_size++;  // ��������� ������� ����


    prolog_size +=  half_size() +       // ������
                    word_size() +       // ����� ���������
                    byte_size() +       // min_instr length
                    byte_size() +       // default is_stmt
                    byte_size() +     	// linebase
                    byte_size() +     	// linerange
                    byte_size();        // opcode base

        // ����� ������� ���������� ����� ��������� ������
    prolog_size += STD_OPCODE_LEN;
    prolog_size += word_size();         // ���� ����� ����� ������

    data = get_new_chunk(elfsectno,prolog_size);
    start_sec = data;

        // ���������� ������ ��������� ��������� data

        // ���� total_length

    dw = 0;		// ����� ����� ��������� ����� ����� ������
                // ������ ���� ����� ������������
    write_4(&dw,data);
    data += sizeof(Dwarf_Word);

    dh = VERSION;
    write_2(&dh,data);
    data += sizeof(Dwarf_Half);

	    // ����� ���������
    du = prolog_size - 
        (word_size() + sizeof(Dwarf_Half) + word_size());

    dw = du;
    write_4(&dw,data);
    data += sizeof(Dwarf_Word);

        // ��������� ���� ���������
    db = MIN_INST_LENGTH;
    memcpy((void *)data,(const void *)&db, sizeof(Dwarf_Ubyte));
    data += byte_size();
    db = DEFAULT_IS_STMT;
    memcpy((void *)data,(const void *)&db, sizeof(Dwarf_Ubyte));
    data += sizeof(Dwarf_Ubyte);
    db = (Dwarf_Ubyte) LINE_BASE;
    memcpy((void *)data,(const void *)&db, sizeof(Dwarf_Ubyte));
    data += byte_size();
    db = LINE_RANGE;
    memcpy((void *)data,(const void *)&db, sizeof(Dwarf_Ubyte));
    data += byte_size();
    db = OPCODE_BASE;
    memcpy((void *)data,(const void *)&db, sizeof(Dwarf_Ubyte));
    data += byte_size();
    memcpy((void *)data,(const void *)Dwarf::std_opcode_len, STD_OPCODE_LEN);
    data += STD_OPCODE_LEN;

        // �������� ���������� ��������
    curdir = cu->dpc_inc_dirs;
    while (curdir) {
        strcpy((char *)data,curdir->dpid_name);
        data += strlen(curdir->dpid_name)+1;
        curdir = curdir->next;
    }

    *data = '\0';   // ��������� ����
    data++;

        // �������� ����� ���������� ������
    curentry = cu->dpc_file_entries;
    while (curentry) {
        strcpy((char *)data,curentry->dpfe_name);
        data += strlen(curentry->dpfe_name)+1;

        memcpy( (void *)data,
                (const void*)curentry->dpfe_args,
                curentry->dpfe_nbytes);

        data += curentry->dpfe_nbytes;
        curentry = curentry->next;
    }
    *data = '\0';
    data++;
    USED(data);

    sum_bytes += prolog_size;

    curline = cu->dpc_lines;
    prevline = new Dwarf_P_Line();

        // �������� ���������� � ������� � ���� ������ ����������� ������ 
    while(curline) {
        int nbytes;
        char *arg;
        Dwarf_Signed opc;

            // ���� ����������� �� �� , ��� �� ����� �������� �������
            // lns_copy, ������������ ���� ����������� ������� ���
            // ������� end_sequence
        Dwarf_Signed no_lns_copy;

        Dwarf_Unsigned addr_adv;
        Dwarf_Signed line_adv;

        no_lns_copy = 0;
        if (curline->dpl_opc != 0) {
            Dwarf_Signed inst_bytes;    // ����� ������ � ����������� �������
            char *str;              // �������� leb128 �������������� ������
            int str_nbytes;         // ����� �������� ������ � str
            switch (curline->dpl_opc) {
                case DW_LNE_end_sequence:

                        // Advance pc to end of text section.
                    addr_adv = curline->dpl_address - prevline->dpl_address;
                    if (addr_adv > 0) {
                        db = DW_LNS_advance_pc;
                        Dwarf::encode_leb128_nm( 
                            addr_adv/MIN_INST_LENGTH,
                            &nbytes,
		                    buff1,
                            sizeof(buff1));

	                    data = get_chunk(elfsectno,nbytes+sizeof(Dwarf_Ubyte));
                        memcpy((void *)data, (const void *)&db,sizeof(Dwarf_Ubyte));
                        data += sizeof(Dwarf_Ubyte);
                        memcpy((void *)data,(const void *)buff1, nbytes);
                        data += nbytes + sizeof(Dwarf_Ubyte);
                        USED(data);
                        sum_bytes += nbytes + sizeof(Dwarf_Ubyte);
                        prevline->dpl_address = curline->dpl_address;
                    }

                    // first null byte

                    db = 0;
                    data = get_chunk(elfsectno,sizeof(Dwarf_Ubyte));
                    memcpy((void *)data,(const void *)&db, sizeof(Dwarf_Ubyte));
                    data += sizeof(Dwarf_Ubyte);
                    USED(data);
                    sum_bytes += sizeof(Dwarf_Ubyte);

                        // write length of extended opcode
                    inst_bytes = sizeof(Dwarf_Ubyte);
                    Dwarf::encode_leb128_nm(inst_bytes,&str_nbytes, buff1,sizeof(buff1));

                    data = get_chunk(elfsectno,str_nbytes);
                    memcpy((void *)data,(const void *)buff1,str_nbytes);
                    data += str_nbytes;
                    USED(data);
                    sum_bytes += str_nbytes;

                        // write extended opcode
                    db = DW_LNE_end_sequence;
                    data = get_chunk(elfsectno,sizeof(Dwarf_Ubyte));
                    memcpy((void *)data,(const void *)&db, sizeof(Dwarf_Ubyte));
                    data += sizeof(Dwarf_Ubyte);
                    USED(data);
                    sum_bytes += sizeof(Dwarf_Ubyte);

                        // reset value to original values
                    prevline->init();
                    no_lns_copy = 1;
                         // this is set only for end_sequence, so that a
                         // dw_lns_copy is not generated
                    break;

				case DW_LNE_set_address:

						//first null byte

					db = 0;
					data = get_chunk(elfsectno,sizeof(Dwarf_Ubyte));
					memcpy((void *)data,(const void *)&db, sizeof(Dwarf_Ubyte));
					data += sizeof(Dwarf_Ubyte);
					USED(data);
					sum_bytes += sizeof(Dwarf_Ubyte);

						// write length of extended opcode
					inst_bytes = sizeof(Dwarf_Ubyte) + word_size();
					Dwarf::encode_leb128_nm(inst_bytes,&str_nbytes,buff1,sizeof(buff1));
					data = get_chunk(elfsectno,str_nbytes);
					str = buff1;
					memcpy((void *)data,(const void *)str,str_nbytes);
					data += str_nbytes;
					USED(data);
					sum_bytes += str_nbytes;

						// write extended opcode
					db = DW_LNE_set_address;
					data = get_chunk(elfsectno,addr_size() + sizeof(Dwarf_Ubyte));
					memcpy((void *)data,(const void *)&db, sizeof(Dwarf_Ubyte));
					data += sizeof(Dwarf_Ubyte);
					sum_bytes += sizeof(Dwarf_Ubyte);

						// store relocation record in linked list
					if (rel_head == NULL) {
						rel_head = new Dwarf_P_Rel();
						rel_tail = rel_head;
					} else {
						rel_tail->next = new Dwarf_P_Rel();
						rel_tail = rel_tail->next;
					}

					rel_tail->set(  
                            sum_bytes,
                            Dwarf_Half(curline->dpl_r_symidx),
                            R_NC_ADDR);
					rel_nbytes += sizeof(Elf32_Rel);

						// write offset
					dw = curline->dpl_address;
					write_4(&dw,data);
					data += sizeof(Dwarf_Word);
					USED(data);
					sum_bytes += sizeof(Dwarf_Word);

					prevline->dpl_address = curline->dpl_address;
					no_lns_copy = 1;
					break;
			}	// end of switch
		}	// end of if(curline->dpl_opc != 0) {}
		else {	// if(curline->dpl_opc == 0)

			if (curline->dpl_file != prevline->dpl_file) {
				// ���� �������� ���� �� ������� ������� DW_LNS_set_file

				db = DW_LNS_set_file;
				Dwarf::encode_leb128_nm(curline->dpl_file,&nbytes,buff1,sizeof(buff1));
				arg = buff1;
				data = get_chunk(elfsectno,nbytes+sizeof(Dwarf_Ubyte));
				memcpy((void *)data,(const void *)&db, sizeof(Dwarf_Ubyte));
				data += sizeof(Dwarf_Ubyte);

				memcpy((void *)data,(const void *)arg, nbytes);
				data += nbytes;
				USED(data);
				sum_bytes += nbytes + sizeof(Dwarf_Ubyte);
				prevline->dpl_file = curline->dpl_file;
			}
			if (curline->dpl_column != prevline->dpl_column) {
				// ���� �������� ����� ������� �� ������� ������� DW_LNS_set_column

				db = DW_LNS_set_column;
				Dwarf::encode_leb128_nm(curline->dpl_column,&nbytes,buff1,sizeof(buff1));

				arg = buff1;
				data = get_chunk(elfsectno,nbytes+sizeof(Dwarf_Ubyte));
				memcpy((void *)data,(const void *)&db, sizeof(Dwarf_Ubyte));
				data += sizeof(Dwarf_Ubyte);
				memcpy((void *)data,(const void *)arg, nbytes);
				data += nbytes;
				USED(data);
				sum_bytes += nbytes + sizeof(Dwarf_Ubyte);
				prevline->dpl_column = curline->dpl_column;
			}

			if (curline->dpl_is_stmt != prevline->dpl_is_stmt) {
				// ���� ���������� ��������� ����� is_statement,
                // �� ������� ������� DW_LNS_negate_stmt

				db = DW_LNS_negate_stmt;
				data = get_chunk(elfsectno,sizeof(Dwarf_Ubyte));
				memcpy((void *)data,(const void *)&db, sizeof(Dwarf_Ubyte));
				data += sizeof(Dwarf_Ubyte);
				USED(data);
				sum_bytes += sizeof(Dwarf_Ubyte);
				prevline->dpl_is_stmt = curline->dpl_is_stmt;
			}

			if (curline->dpl_basic_block == True 
                            && prevline->dpl_basic_block == False) {
				// ���� ������� ������� ���� ������� 
                // ������� DW_LNS_set_basic_block

				db = DW_LNS_set_basic_block;
				data = get_chunk(elfsectno,sizeof(Dwarf_Ubyte));
				memcpy((void *)data,(const void *)&db, sizeof(Dwarf_Ubyte));
				data += sizeof(Dwarf_Ubyte);
                USED(data);
				sum_bytes += sizeof(Dwarf_Ubyte);
				prevline->dpl_basic_block = curline->dpl_basic_block;
			}

			addr_adv = curline->dpl_address - prevline->dpl_address;
			line_adv = curline->dpl_line - prevline->dpl_line;

                // � ��� ����������� ������ ���������� 1
                // ���, ��� ���� �� ������������ 
                // ( ��������� �� ������� )
			if ((addr_adv % MIN_INST_LENGTH) != 0)
				throw Dwarf_Error_Pro_Line("line encoding: bad address increment");

            if ((opc = Dwarf::get_opc(addr_adv,line_adv)) > 0) {
                no_lns_copy = 1;
                db = Dwarf_Ubyte(opc);
                data = get_chunk(elfsectno,sizeof(Dwarf_Ubyte));
                memcpy((void *)data,(const void *)&db, sizeof(Dwarf_Ubyte));
                data += sizeof(Dwarf_Ubyte);
                USED(data);
                sum_bytes += sizeof(Dwarf_Ubyte);
                prevline->dpl_basic_block = False;
                prevline->dpl_address = curline->dpl_address;
                prevline->dpl_line = curline->dpl_line;
            }
			else {
				if (addr_adv > 0) {
					db = DW_LNS_advance_pc;
					Dwarf::encode_leb128_nm(addr_adv/MIN_INST_LENGTH,&nbytes,
						buff1,sizeof(buff1));

					arg = buff1;
					data = get_chunk(elfsectno,nbytes+sizeof(Dwarf_Ubyte));
					memcpy((void *)data, (const void *)&db, sizeof(Dwarf_Ubyte));
					data += sizeof(Dwarf_Ubyte);
					memcpy((void *)data,(const void *)arg, nbytes);
					data += nbytes + sizeof(Dwarf_Ubyte);
					USED(data);
					sum_bytes += nbytes + sizeof(Dwarf_Ubyte);
					prevline->dpl_basic_block = False;
					prevline->dpl_address = curline->dpl_address;
				}
				if (line_adv != 0) {
					db = DW_LNS_advance_line;
					Dwarf::encode_signed_leb128_nm(line_adv, &nbytes,buff1,sizeof(buff1));

					arg = buff1;
					data = get_chunk(elfsectno,nbytes+sizeof(Dwarf_Ubyte));
					memcpy((void *)data, (const void *)&db, sizeof(Dwarf_Ubyte));
					data += sizeof(Dwarf_Ubyte);
					memcpy((void *)data,(const void *)arg, nbytes);
					data += nbytes + sizeof(Dwarf_Ubyte);
					USED(data);
					sum_bytes += nbytes + sizeof(Dwarf_Ubyte);
					prevline->dpl_basic_block = False;
					prevline->dpl_line = curline->dpl_line;
				}
			}
		} // ends else for opc != 0

		if (no_lns_copy == 0) {	// if not a special or dw_lne_end_seq
										//	generate a matrix line
			db = DW_LNS_copy;
			data = get_chunk(elfsectno,sizeof(Dwarf_Ubyte));
			memcpy((void *)data,(const void *)&db, sizeof(Dwarf_Ubyte));
			data += sizeof(Dwarf_Ubyte);
			USED(data);
			sum_bytes += sizeof(Dwarf_Ubyte);
			prevline->dpl_basic_block = False;
		}
		curline = curline->next;

	} // ends of while(curline)

    delete prevline;    // ������ �� �����

		// write total length field
    du = sum_bytes - word_size(); // subtract length field

    dpd_next_line_offset = du;
    dw = du;
    write_4(&dw,start_sec);

	if (rel_nbytes > 0) {

		Dwarf::reloc_sects[DEBUG_LINE] = dpd_func(".rel.debug_line",SHT_REL, 0,
			SHN_UNDEF,Dwarf::elf_sects[DEBUG_LINE],&name_idx);

		if (Dwarf::reloc_sects[DEBUG_LINE] == -1)
			throw Dwarf_Error_Pro_Elf("error creating section .rel.debug_line");

            // write out relocation records
		elfsectno = Dwarf::reloc_sects[DEBUG_LINE];
		write_reloc_section(rel_head,elfsectno,rel_nbytes);

        delete rel_head;    // ������� ����� ����� �� �����
	}

	return dpd_n_debug_sect;
}
