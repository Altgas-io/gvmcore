//////////////////////////////////////////////////////////////////////////
//                                                                      //
//            ������� ����������� ����������� ���������������           //
//                                                                      //
//        ���������� ������� � ���������� ���������� ������� DWARF      //
//                                                                      //
//                ������������ ���� pro_pubn.hpp                        //
//                                                                      //
//  ���� ���������� ��������� �����  ���������� ��������                //
//  Dwarf_P_Pubname                                                     //
//                                                                      //
//                                                                                         //
//  If this code works, it was                                                         //
//  If not, I don't know who wrote it.                                  //
//                                                                      //
// $Revision:: 1     $      $Date:: 7/21/05 5:32p    $                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class Dwarf_P_Pubname {
    private:
        Dwarf_P_Die *dpp_die;   // ������ �� DIE ��������� � ������ ��������
        char        *dpp_name;  // ��� ����������� �������

    public:
        Dwarf_P_Pubname *next;  // ��������� ������� ������ ���������� ��������

        Dwarf_P_Pubname(Dwarf_P_Die *die, char *name) : dpp_die(die),
                                                      dpp_name(name),
                                                      next(NULL) { }

        ~Dwarf_P_Pubname()
        {
            if (dpp_name) delete dpp_name;
            if (next) delete next;
        }

      Dwarf_P_Die *die() const { return dpp_die; }

      char *name() const { return dpp_name; }
};
