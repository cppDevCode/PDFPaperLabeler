/* Etiq+: Etiquetador de hojas
* Version 0.95.1l
* Copyright 2019-2020 Alejandro Lucas Baldres
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

#include <gtk/gtk.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "cups/cups.h" //API CUPS
#include "impresora.h"
#include "generoPDF.h" //Generacion del PDF para imprimir

/* Declaracion de Objetos y variables globales
*/ 
GtkLabel *lblNombre;
GtkLabel *labEstado;
GtkLabel *labMarca;
GtkLabel *labColegio;
GtkLabel *labNombColegio;
GtkLabel *labMateria;
GtkLabel *labHoja;
GtkLabel *labEColegio;
GtkComboBoxText *txtNombre;
GtkComboBoxText *txtMarca;
GtkEntry *txtAno;
GtkEntry *txtMateria;
GtkButton *btnImprimir;
GtkButton *btnCancelar;
GtkSpinButton *spDesde;
GtkSpinButton *spHasta;
GtkTextBuffer *txtBuffer;
GtkLinkButton *linkImpresora;
GtkComboBoxText *cmbhoja;
GtkEntry *txtColegio;
GtkImage *imghoja;
GtkLabel *labadvertencia;
GtkButton *btnSi;
GtkButton *btnNo;
GtkDialog *dlgAdvertencia;
GtkImage *imgSello;
GtkBuilder      *advertencia;
int AdvertenciaCreada=0;
int ventanaImpresora=0; //Determina si la ventana de selector de impresora fue creada
GtkWidget *wmain;

//Ventana Advertencia




/* Declaracion de prototipos
*/
int mkdir (const char *pathname, mode_t mode);
int creoDirectorioOdt (char directorioBase[255], char *subdirectorio);
int Creo_advertencia(GtkBuilder *advertencia, int Error);

/* Funcion Principal donde se inicializan los objetos y
*  conectan con las señales
*/
int main (int argc, char *argv[])
{
    GtkBuilder *builder; 
    cups_dest_t *dests;
    
    
    char  dirBase[100] = "/tmp/etiquetador.";
 
    gtk_init (&argc, &argv);
 
    builder = gtk_builder_new ();
    
    gtk_builder_add_from_file (builder, "glade/main.glade", NULL);
     
    lblNombre = GTK_LABEL (gtk_builder_get_object(builder, "lblNombre"));
    labEstado = GTK_LABEL (gtk_builder_get_object(builder, "labEstado"));
    txtNombre = GTK_COMBO_BOX_TEXT (gtk_builder_get_object(builder, "cmbNombre"));
    txtAno = GTK_ENTRY (gtk_builder_get_object(builder, "txtAno"));
    txtMateria = GTK_ENTRY (gtk_builder_get_object(builder, "txtMateria"));
    btnCancelar = GTK_BUTTON (gtk_builder_get_object(builder, "btnCancelar"));
    btnImprimir = GTK_BUTTON (gtk_builder_get_object(builder, "btnImprimir"));
    spDesde = GTK_SPIN_BUTTON (gtk_builder_get_object(builder, "spDesde"));
    spHasta = GTK_SPIN_BUTTON (gtk_builder_get_object(builder, "spHasta"));
    txtBuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "txtBuffer"));
    linkImpresora = GTK_LINK_BUTTON (gtk_builder_get_object(builder, "linkImpresora"));
    cmbhoja = GTK_COMBO_BOX_TEXT (gtk_builder_get_object(builder,"cmbhoja"));
    txtMarca = GTK_COMBO_BOX_TEXT (gtk_builder_get_object(builder, "cmbMarca"));
    txtColegio = GTK_ENTRY (gtk_builder_get_object(builder, "txtColegio"));
    imghoja = GTK_IMAGE (gtk_builder_get_object(builder,"imghoja"));
    labColegio = GTK_LABEL (gtk_builder_get_object(builder, "labColegio"));
    labHoja = GTK_LABEL (gtk_builder_get_object(builder, "labHoja"));
    labMateria = GTK_LABEL (gtk_builder_get_object(builder, "labMateria"));
    labNombColegio = GTK_LABEL (gtk_builder_get_object(builder, "labNombColegio"));
    labEColegio = GTK_LABEL (gtk_builder_get_object(builder,"labEColegio"));
    labMarca = GTK_LABEL (gtk_builder_get_object(builder,"labMarca"));
    imgSello = GTK_IMAGE (gtk_builder_get_object(builder, "imgSello"));
    wmain = GTK_WIDGET(gtk_builder_get_object(builder,"main"));

    gtk_builder_connect_signals (builder, NULL);
            
    g_object_unref (builder);
    
    /*Se pide a CUPS la impresora por defecto, en caso de no 
    * estar devuelve NULL y abre la ventana de selector de impresora
    */
    if (cupsGetDests(&dests) == 0)
    {
        printf ("ERROR: NO EXISTEN IMPRESORAS INSTALADAS");
        gtk_widget_hide(GTK_WIDGET(wmain));
        Creo_advertencia(advertencia,1);
        gtk_widget_show(GTK_WIDGET(dlgAdvertencia));
    }
    else
        {
        //
        if (cupsGetDefault() == NULL)
            {
               ventanaImpresora = creoSelectorImpresora(1);
            }
        else 
            {
                Impresora = (char *)cupsGetDefault();
                strcpy(link2,"");
                strcat(link2,"http://localhost:631/printers/");
                strcat(link2,Impresora);
                gtk_button_set_label(GTK_BUTTON(linkImpresora),Impresora);
                gtk_link_button_set_uri(linkImpresora,link2);
            }
        }
    //Se cambia el nombre de la label por el de la impresora seleccionada
    gtk_button_set_label(GTK_BUTTON(linkImpresora),Impresora);
    printf(cupsUserAgent ());
    creoDirectorioOdt (dirBase, getlogin ()); //Creo directorio temporal
    Marca=0;
    gtk_main ();
   
    return 0;
}

int Creo_advertencia(GtkBuilder *advertencia, int Error)
{
  
    GtkLabel *labTitulo;
    advertencia = gtk_builder_new ();
    
    gtk_builder_add_from_file (advertencia, "glade/advertencia.glade", NULL);
    labadvertencia = GTK_LABEL(gtk_builder_get_object(advertencia,"labDetalle"));
    labTitulo = GTK_LABEL(gtk_builder_get_object(advertencia,"labTitulo"));
    btnSi = GTK_BUTTON(gtk_builder_get_object(advertencia,"btnHacer"));
    btnNo = GTK_BUTTON(gtk_builder_get_object(advertencia,"btnNo"));
    dlgAdvertencia= GTK_DIALOG(gtk_builder_get_object(advertencia,"dlgAdvertencia"));
    gtk_builder_connect_signals (advertencia, NULL);
    if (Error == 1)
    {
        gtk_widget_hide(GTK_WIDGET(btnSi));
        gtk_widget_hide(GTK_WIDGET(labadvertencia));
        gtk_button_set_label(btnNo,"SALIR");
        gtk_label_set_text(labTitulo,"IMPRESORA NO INSTALADA");        
        gtk_label_set_text(labadvertencia,"1");
    }        
    g_object_unref (advertencia);
    return 1;
}

/* Funcion utilizada para que se refleje cualquier cambio en el tipeo
* de datos en el previsualizador
*/
void on_cmbNombre_changed ()
{
    int cantidad;
    gchar hojaInicial[100]={0};
    
    if (gtk_spin_button_get_value (spHasta) < gtk_spin_button_get_value (spDesde))
    {
        gtk_spin_button_set_value(spHasta,gtk_spin_button_get_value (spDesde));
    }
    
    cantidad = gtk_spin_button_get_value_as_int (spDesde);
    g_snprintf (hojaInicial, sizeof (hojaInicial), "%d", cantidad);
    if (strcmp(gtk_entry_get_text(txtMateria),"") != 0)
    {
        gtk_label_set_text (lblNombre, g_strjoin (NULL, gtk_combo_box_text_get_active_text (txtNombre), "\n", gtk_entry_get_text (txtAno), "\n", gtk_entry_get_text (txtMateria), "\n", "Hoja Nº ", hojaInicial, (gchar*)(char*)NULL));
        gtk_label_set_text(labNombColegio,g_strjoin (NULL, gtk_combo_box_text_get_active_text (txtNombre), "\n", gtk_entry_get_text (txtAno),(gchar*)(char*)NULL));
        gtk_label_set_text(labColegio,gtk_entry_get_text(txtColegio));
        gtk_label_set_text(labHoja,hojaInicial);
        gtk_label_set_text(labMateria,gtk_entry_get_text (txtMateria));
    }
    else
    {
        gtk_label_set_text (lblNombre, g_strjoin (NULL, gtk_combo_box_text_get_active_text (txtNombre), "\n", gtk_entry_get_text (txtAno), "\n", "Hoja Nº ", hojaInicial, (gchar*)(char*)NULL));
        gtk_label_set_text(labNombColegio,g_strjoin (NULL, gtk_combo_box_text_get_active_text (txtNombre), "\n", gtk_entry_get_text (txtAno), (gchar*)(char*)NULL));
        gtk_label_set_text(labColegio,gtk_entry_get_text (txtColegio));
        gtk_label_set_text(labHoja,hojaInicial);
        gtk_label_set_text(labMateria,gtk_entry_get_text (txtMateria));
    }
    
}
//Funcion que establece la marca de Hoja Nº3 a fin de determinar el lugar de impresión
//0: Rivadavia 1: Exito
void cmbMarca_changed_cb()
{
    switch (gtk_combo_box_get_active(GTK_COMBO_BOX(txtMarca)))
        {
        case 0:
            Marca=0;
            break;
    
        case 1:
            Marca=1;
            break;
        default:
            break;
        }   
}

//Función que al seleccionar el tipo de hoja cambia la previsualizacion

void on_cmbhoja_changed()
{
switch (gtk_combo_box_get_active(GTK_COMBO_BOX(cmbhoja)))
{
case 0:
    gtk_image_set_from_file(imghoja,"glade/hojas.png");
     gtk_widget_show(GTK_WIDGET(lblNombre));
     gtk_widget_show(GTK_WIDGET(txtMarca));
    gtk_widget_show(GTK_WIDGET(labMarca));
    gtk_widget_show(GTK_WIDGET(imgSello));
    gtk_widget_hide(GTK_WIDGET(labColegio));
    gtk_widget_hide(GTK_WIDGET(labHoja));
    gtk_widget_hide(GTK_WIDGET(labMateria));
    gtk_widget_hide(GTK_WIDGET(labNombColegio));
    gtk_widget_hide(GTK_WIDGET(labEColegio));
    gtk_widget_hide(GTK_WIDGET(txtColegio));
    break;
case 1:
    gtk_image_set_from_file(imghoja,"glade/hojaavon.png");
    gtk_widget_hide(GTK_WIDGET(lblNombre));
    gtk_widget_hide(GTK_WIDGET(txtMarca));
    gtk_widget_hide(GTK_WIDGET(labMarca));
    gtk_widget_hide(GTK_WIDGET(imgSello));
    gtk_widget_show(GTK_WIDGET(labColegio));
    gtk_widget_show(GTK_WIDGET(labHoja));
    gtk_widget_show(GTK_WIDGET(labMateria));
    gtk_widget_show(GTK_WIDGET(labNombColegio));
    gtk_widget_show(GTK_WIDGET(labEColegio));
    gtk_widget_show(GTK_WIDGET(txtColegio));
    break;
default:
    break;
}
}
void on_main_destroy ()
{
    gtk_main_quit ();
}



/* CreoDirectorioOdt
*  Genera un directorio dado
*  Regresa 0 si lo creo o 1 si este existe
*/
int creoDirectorioOdt (char directorioBase[255], char *subdirectorio)
{
    int directorioCreado = 0;
    char aux[255] = "";
    
    strcpy (aux, directorioBase);
    strcat (aux, subdirectorio);
    directorioCreado = mkdir (aux, S_IRWXU);

    return directorioCreado;

}


//Función que inicia el proceso de generacion de archivo e impresión
void on_btnImprimir_clicked ()
{
    gchar desde[100]={0};
    gchar hasta[100]={0};
    int temp;

     temp=gtk_spin_button_get_value_as_int (spDesde);
     g_snprintf (desde, sizeof (desde), "%d", temp); 
     temp=gtk_spin_button_get_value_as_int (spHasta);
     g_snprintf (hasta, sizeof (hasta), "%d", temp);  
    if (AdvertenciaCreada==0)
    {
        printf("\n\nEntro");
        AdvertenciaCreada=Creo_advertencia(advertencia,0);
        AdvertenciaCreada=1;
    }
    gtk_widget_hide(wmain);
    gtk_widget_show(GTK_WIDGET(dlgAdvertencia));
    switch (gtk_combo_box_get_active(GTK_COMBO_BOX(cmbhoja)))
    {
    case 0:
         switch (Marca)
         {
         case 0 /* Rivadavia */:
            if (strcmp(gtk_entry_get_text(txtMateria),"") != 0)
                 gtk_label_set_text (labadvertencia, g_strjoin (NULL, "TIPO DE HOJA: CARPETA Nª3\nMARCA: RIVADAVIA\nNOMBRE: ",gtk_combo_box_text_get_active_text (txtNombre), "\nCURSO: ", gtk_entry_get_text (txtAno), "\nMATERIA: ",gtk_entry_get_text(txtMateria), "\nHOJA INICIAL: ",desde,"\nHOJA FINAL: ",hasta, (gchar*)(char*)NULL));
            else
            {
                gtk_label_set_text (labadvertencia, g_strjoin (NULL, "TIPO DE HOJA: CARPETA Nª3\nMARCA: RIVADAVIA\nNOMBRE: ",gtk_combo_box_text_get_active_text (txtNombre), "\nCURSO: ", gtk_entry_get_text (txtAno), "\nHOJA INICIAL: ",desde,"\nHOJA FINAL: ",hasta, (gchar*)(char*)NULL));
            }
            
             break;
         case 1 /*exito*/:
            if (strcmp(gtk_entry_get_text(txtMateria),"") != 0)
                 gtk_label_set_text (labadvertencia, g_strjoin (NULL, "TIPO DE HOJA: CARPETA Nª3\nMARCA: EXITO\nNOMBRE: ",gtk_combo_box_text_get_active_text (txtNombre), "\nCURSO: ", gtk_entry_get_text (txtAno), "\nMATERIA: ",gtk_entry_get_text(txtMateria), "\nHOJA INICIAL: ",desde,"\nHOJA FINAL: ",hasta, (gchar*)(char*)NULL));
            else
            {
                gtk_label_set_text (labadvertencia, g_strjoin (NULL, "TIPO DE HOJA: CARPETA Nª3\nMARCA: EXITO\nNOMBRE: ",gtk_combo_box_text_get_active_text (txtNombre), "\nCURSO: ", gtk_entry_get_text (txtAno), "\nHOJA INICIAL: ",desde,"\nHOJA FINAL: ",hasta, (gchar*)(char*)NULL));
            }
         default:
             break;
         }  
        break;
    case 1:
        if (strcmp(gtk_entry_get_text(txtMateria),"") != 0)
                 gtk_label_set_text (labadvertencia, g_strjoin (NULL, "TIPO DE HOJA: PAPEL AVON (CUADERNO)\nNOMBRE: ",gtk_combo_box_text_get_active_text (txtNombre), "\nCURSO: ", gtk_entry_get_text (txtAno), "\nMATERIA: ",gtk_entry_get_text(txtMateria),"\nCOLEGIO: ",gtk_entry_get_text (txtColegio), "\nHOJA INICIAL: ",desde,"\nHOJA FINAL: ",hasta, (gchar*)(char*)NULL));
            else
            {
                gtk_label_set_text (labadvertencia, g_strjoin (NULL, "TIPO DE HOJA: PAPEL AVON (CUADERNO)\nNOMBRE: ",gtk_combo_box_text_get_active_text (txtNombre), "\nCURSO: ", gtk_entry_get_text (txtAno),"\nCOLEGIO: ",gtk_entry_get_text (txtColegio), "\nHOJA INICIAL: ",desde,"\nHOJA FINAL: ",hasta, (gchar*)(char*)NULL));
            }
        break;
    default:
        break;
    }
    
}

void on_btnHacer_clicked()
{
    switch (gtk_combo_box_get_active(GTK_COMBO_BOX(cmbhoja)))
    {
    case 0:
            generoPDF (gtk_combo_box_text_get_active_text(txtNombre),(char *)gtk_entry_get_text(txtMateria),(char *)gtk_entry_get_text(txtAno),gtk_spin_button_get_value_as_int (spDesde),gtk_spin_button_get_value_as_int (spHasta),"",0);     
        break;
    case 1:
        generoPDF (gtk_combo_box_text_get_active_text(txtNombre),(char *)gtk_entry_get_text(txtMateria),(char *)gtk_entry_get_text(txtAno),gtk_spin_button_get_value_as_int (spDesde),gtk_spin_button_get_value_as_int (spHasta),(char *)gtk_entry_get_text(txtColegio),1);     
        break;
    default:
        break;
    }
    gtk_widget_hide(GTK_WIDGET(dlgAdvertencia));
    gtk_widget_show(wmain);
}

void on_btnNo_clicked()
{
    char *tmp;

    tmp = gtk_label_get_text(labadvertencia);
    if (strcmp(tmp,"1") == 0)
        {
        on_main_destroy();
        }
    else
        {
            gtk_widget_hide(GTK_WIDGET(dlgAdvertencia));
            gtk_widget_show(wmain);
        }
}
void on_btnImpresora_clicked ()
{
    if (ventanaImpresora)
    {
        mostrar_selector();
    }
    else
    {
        ventanaImpresora = creoSelectorImpresora(0);    
    } 

}

void on_spHasta_value_changed()
{
    if (gtk_spin_button_get_value (spHasta) < gtk_spin_button_get_value (spDesde))
    {
        gtk_spin_button_set_value(spHasta,gtk_spin_button_get_value (spDesde));
    }
}
