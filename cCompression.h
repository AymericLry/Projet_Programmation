#ifndef CCOMPRESSION_H
#define CCOMPRESSION_H
#include <iostream>

using namespace std;

const unsigned taille = 8;

class cCompression
{
    unsigned int mLargeur;  //La largeur de l'image
    unsigned int mHauteur;  //La hauteur de l'image
    unsigned int **mBuffer;    //L'image
    unsigned int mQualite;      //Qualité de la compression (0 à 100) 50 par défaut
    int Q[taille][taille] = {
                                { 16, 11, 10, 16, 24,  40,  51,  61  },
                                { 12, 12, 14, 19, 26,  58,  60,  55  },
                                { 14, 13, 16, 24, 40,  57,  69,  56  },
                                { 14, 17, 22, 29, 51,  87,  80,  62  },
                                { 18, 22, 37, 56, 68,  109, 103, 77  },
                                { 24, 35, 55, 64, 81,  104, 113, 92  },
                                { 49, 64, 78, 87, 103, 121, 120, 101 },
                                { 72, 92, 95, 98, 112, 100, 103, 99  },
                            };

    public:
        cCompression(unsigned int, unsigned int, int Bloc8x8[][taille], unsigned int qualite = 50);
        virtual ~cCompression();
        unsigned int GetLageur() const;
        unsigned int GetHauteur() const;
        unsigned int GetQualite() const;
        void SetLageur(unsigned);
        void SetHauteur(unsigned);
        void SetQualite(unsigned);
        void Calcul_DCT_Block(double DCT_Img[][taille], int Bloc8x8[][taille]);
        void Calcul_IDCT(double DCT_Img[][taille], int Bloc8x8[][taille]);
        void quant_JPEG(double Img_DCT[][taille], int Img_Quant[][taille]);
        void creationQtab(int Qtab[][taille]);
        void dequant_JPEG(double Img_DCT[][taille], int Img_Quant[][taille]);
        double EQM(int Img_IDCT[][taille]);
        double taux_compression(double Bloc8x8[][taille]);
        //void RLE\_Block(int Bloc8x8[][taille], int, char);
        void Testalgo();

        template <typename T>
        void Affichage(T img[][taille]) const
        {
            cout << "[  ";
            for(unsigned i=0; i<taille; i++)
            {
                for(unsigned j=0; j<taille; j++)
                {
                    if(j<taille-1)
                    {
                        cout << img[i][j] << ",   ";
                    }
                    else
                    {
                        cout << img[i][j];
                    }
                }
                if(i<taille-1)
                {
                    cout << "  ]" << endl << "[  ";
                }
                else
                {
                    cout << "  ]" << endl;
                }
            }
        }

};

#endif // CCOMPRESSION_H
