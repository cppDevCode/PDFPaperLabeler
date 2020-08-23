/* Etiq+: Etiquetador de hojas Nº 3 y Obra
* Copyright 2019 Alejandro Lucas Baldres
*
*
* Etiq+ is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* Etiq+ is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Etiq+.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <hpdf.h>
#include <string.h>
#include <hpdf_types.h>
#include <setjmp.h>
#include "impresora.h"
#include <gtk/gtk.h>

jmp_buf env;

int print_page  (HPDF_Page page, HPDF_Font font, const char * nombre, const char *materia, const char *curso, const char *nroHoja);
int imprimo_texto (HPDF_Page pagina, HPDF_Font fuente, HPDF_REAL alturaHoja,const char *texto);
int print_page_obra  (HPDF_Page page, HPDF_Font font, const char * nombre, const char *materia, const char *curso, const char *Colegio, const char *nroHoja);
int imprimo_texto_obra (HPDF_Page pagina, HPDF_Font fuente, HPDF_REAL ancho, HPDF_REAL alturaHoja,const char *texto);
void error_handler (HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data);
int Marca;
HPDF_Doc pdf;
draw_image (HPDF_Doc     pdf,
            const char  *filename,
            float        x,
            float        y);

/* GeneroPDF
*  Genera los directorios en la carpeta temporal:
*  en Linux: /tmp/etiquetador.[Nombre_usuario]
*  y llama a las funciones de generacion de archivos
*/
/*Tipo Hoja:
*  0: Hoja Nro 3
*  1: Tipo Obra (223 x 295 mm)
*/

int generoPDF (gchar *nombre, gchar *materia, gchar *curso,int pagInicial, int pagFinal, gchar *Colegio, int TipoHoja)
{
    char  dirBase[100] = "";
    char  dirFuente[255] = "";
    HPDF_Font fuente;
    const char *nombreFuente;
    HPDF_Page paginas[40];
    char nroHoja[12]="";
    int i,f;

    printf("Marca=%d",Marca);
  /*  strcat(dirBase,"/tmp/etiquetador.");
    strcat(dirBase,getlogin());
    strcat(dirBase,"/out.pdf");*/

     if (setjmp(env)) {
        HPDF_Free (pdf);
        return 1;
    }
    pdf = HPDF_New (error_handler, NULL);
    if (!pdf) {
        printf ("ERROR: cannot create pdf object.\n");
        return 1;
    }

    printf("/* set compression mode */\n");
    HPDF_SetCompressionMode (pdf, HPDF_COMP_ALL);

    printf("/* set page mode to use outlines. */");
    HPDF_SetPageMode (pdf, HPDF_PAGE_MODE_USE_OUTLINE);
    //sprintf(nroHoja, "%d\0", pagFinal);
    f=pagFinal;
    printf("f=%i",f);
    strcat(dirFuente,"/home/");
    strcat(dirFuente,getlogin());
    strcat(dirFuente,"/.fonts/Carlito-Bold.ttf");
    printf("\n\nDIR FUENTE: %s",dirFuente);
    nombreFuente = HPDF_LoadTTFontFromFile (pdf, dirFuente, HPDF_TRUE);
    
    for ( i = 0; i < pagFinal-pagInicial+1; i++)
    {
        paginas[i] = HPDF_AddPage (pdf);
        fuente = HPDF_GetFont (pdf, nombreFuente, NULL);
        switch (TipoHoja)
        {
        case 0:
            sprintf(nroHoja, "Hoja Nro %d%c", f,'\0');
            print_page(paginas[i],fuente,nombre,materia,curso,nroHoja);    
            printf("PAGINAS:%d\nFUENTE:%s\nNOMBRE:%s\nMATERIA:%s\nCURSO: %s\nHOJAS: %s",paginas[i],nombreFuente,nombre,materia,curso,nroHoja);
            break;
        case 1:
            sprintf(nroHoja, "%d%c", f,'\0');
            print_page_obra(paginas[i],fuente,nombre,materia,curso,Colegio,nroHoja);    
           // printf("PAGINAS:%d\nFUENTE:%s\nNOMBRE:%s\nMATERIA:%s\nCURSO: %s\nHOJAS: %s",paginas[i],nombreFuente,nombre,materia,nroHoja);
            break;

        default:
            break;
        }
        
        f--;
    }
    
    strcat(dirBase,"/tmp/etiquetador.");
    strcat(dirBase,getlogin());
    strcat(dirBase,"/out.pdf");

    printf("\n\nDIRBASE:%s",dirBase);
    HPDF_SaveToFile (pdf,dirBase);
    printf("\n\nSII");
    HPDF_Free (pdf);
    cupsPrintFile(Impresora,dirBase,"etiquetado",0,NULL);
    
    
    return 0;
    
}

void error_handler (HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data)
{
    printf ("ERROR: error_no=%04X, detail_no=%d\n",
      (unsigned int) error_no, (int) detail_no);
    longjmp (env, 1); /* invoke longjmp() on error */
}

int print_page  (HPDF_Page page, HPDF_Font font, const char * nombre, const char *materia, const char *curso, const char *nroHoja)
{
    HPDF_REAL base = 630.7559;

    HPDF_Page_SetSize (page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
    HPDF_Page_SetWidth (page,538.583);
    HPDF_Page_SetHeight(page,680.315);
    HPDF_Page_SetFontAndSize (page, font, 8);
    ///hoja rivadavia base = 630.7559;
    //exito mide la altura donde empezar a poner el texto
    printf("\n\nMARCA HOJA=%d",Marca);
    if (Marca == 0)
    {
        //Marca Rivadavia
        base = 630.7559;
    }
    else
    {
        //Marca Exito
        base = 566.928776;    
    }
    printf("\nBASE=%f",base);
    
    imprimo_texto(page,font,base,nombre);
    if (strcmp(materia,"") != 0)
    {
        base = base - 8;
        imprimo_texto(page,font,base,materia);
        printf("\nMateria=%s",materia);
    }
    printf("\nMateria1=%s",materia);
    base = base - 8;
    imprimo_texto(page,font,base,curso);
    printf("\nMateria1=%s",materia);
    base = base - 8;
    imprimo_texto(page,font,base,nroHoja);
    printf("\nMateria2=%s",materia);
    base = base - 60;
    draw_image(pdf,"/home/alejandro/Documentos/Programacion/Proyectos/etiquetador/glade/sello.png",9.937008,base);
    return 0;
}

int imprimo_texto (HPDF_Page pagina, HPDF_Font fuente, HPDF_REAL alturaHoja,const char *texto)
{
    HPDF_Page_BeginText (pagina);
    HPDF_Page_MoveTextPos2 (pagina, 7.937008, alturaHoja);
    HPDF_Page_ShowText (pagina, texto);
    HPDF_Page_EndText (pagina);   
    return 0;
}

int print_page_obra  (HPDF_Page page, HPDF_Font font, const char * nombre, const char *materia, const char *curso, const char *Colegio, const char *nroHoja)
{
    HPDF_REAL base = 805.05459;
    HPDF_REAL ancho = 62.362165;

  /* 1 pixel = 0.352778 mm at 72 dpi */
    HPDF_Page_SetSize (page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
    //HPDF_Page_SetWidth (page,632.12559);
    //HPDF_Page_SetHeight(page,836.21995);
    HPDF_Page_SetFontAndSize (page, font, 8);
    base = 805.05459;
    ancho = 70.866097;
    imprimo_texto_obra(page,font,ancho, base,Colegio);
    if (strcmp(materia,"") != 0)
    {
        base = base - 10;
        ancho=240.94473;
        HPDF_Page_SetFontAndSize(page,font,14);
        imprimo_texto_obra(page,font,ancho,base,materia);
        base = base + 10;
    }
    base = base - 10;
    HPDF_Page_SetFontAndSize(page,font,14);
    ancho = 70.866097;
    imprimo_texto_obra(page,font,ancho,base,nombre);
    base = base - 15;
    imprimo_texto_obra(page,font,ancho,base,curso);
    base = 802.204219;
    ancho = 520.04695;
    HPDF_Page_SetFontAndSize(page,font,10);
    imprimo_texto_obra(page,font,ancho,base,nroHoja);
    return 0;
}



int imprimo_texto_obra (HPDF_Page pagina, HPDF_Font fuente, HPDF_REAL ancho, HPDF_REAL alturaHoja,const char *texto)
{
    HPDF_Page_BeginText (pagina);
    HPDF_Page_MoveTextPos2 (pagina, ancho, alturaHoja);
    HPDF_Page_ShowText (pagina, texto);
    HPDF_Page_EndText (pagina);   
    
    return 0;
}

draw_image (HPDF_Doc     pdf,
            const char  *filename,
            float        x,
            float        y)
{
    HPDF_Page page = HPDF_GetCurrentPage (pdf);
    HPDF_Image image;

     image = HPDF_LoadPngImageFromFile (pdf, "/home/alejandro/Documentos/Programacion/Proyectos/etiquetador/glade/sello.png");

    /* Draw image to the canvas. */
    HPDF_Page_DrawImage (page, image, x, y, 40, 46);

}