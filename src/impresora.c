/* Selector de impresora
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
#include <unistd.h>
#include <stdio.h>
#include "cups/cups.h"
#include <stdlib.h>
#include <sys/stat.h>
#include "main.h"

GtkDialog *selImpresora;
GtkComboBoxText *cmbImpresora;
GtkButton *btnCancelar;
GtkImage *imgImpresora;
GtkLabel *labTituloImpresora;
gint item=1;  
char *Impresora="";
char link2[100]={""};
cups_dest_t *Destinos;
int cantDestinos=0;

//Estructura de datos segun API CUPS: https://www.cups.org/doc/cupspm.html
 struct Impresoras
    {
        char* Nombre;
        char* Modelo;
        char* Informacion;
        char* Localizacion;
    } DetalleImpresora[10];

int creoSelectorImpresora(int inhiboCancelar)
{
    GtkBuilder      *builder; 
    
    cups_dest_t *DefaultImpresora;
    gchar *TextImpresora;
    char* ptr;
    
   

    for (size_t i = 0; i < 11; i++)
    {
        DetalleImpresora[i].Nombre="";
        DetalleImpresora[i].Modelo="";
        DetalleImpresora[i].Informacion="";
        DetalleImpresora[i].Localizacion="";
    }
    
    
    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "glade/selimpresora.glade", NULL);
     
    cmbImpresora = GTK_COMBO_BOX_TEXT (gtk_builder_get_object(builder, "cmbImpresora"));
    selImpresora = GTK_DIALOG (gtk_builder_get_object(builder, "dlgImpresora"));
    btnCancelar = GTK_BUTTON (gtk_builder_get_object(builder, "btnCancelar"));
    labTituloImpresora = GTK_LABEL (gtk_builder_get_object(builder, "labTituloImpresora"));
    imgImpresora = GTK_IMAGE (gtk_builder_get_object(builder,"imgImpresora"));

   
    gtk_builder_connect_signals (builder, NULL);
            
    g_object_unref (builder);
    //Consulto cual es la impresora por defecto del sistema
    //cantDestinos = cupsGetDests (&Destinos);
    cantDestinos = cupsGetDests2(CUPS_HTTP_DEFAULT,&Destinos);
    //printf(cantDestinos);
   // DefaultImpresora = cupsGetDest(NULL,NULL,cantDestinos,Destinos);
    //TextImpresora =(char *)cupsGetOption("printer-make-and-model",DefaultImpresora->num_options,DefaultImpresora->options);
    
    
    for (int i = 0; i < cantDestinos; i++)
    {
        gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (cmbImpresora), Destinos[i].name);
        gtk_combo_box_set_active (GTK_COMBO_BOX (cmbImpresora), 0); 
        DetalleImpresora[i].Nombre=Destinos[i].name;
        DetalleImpresora[i].Modelo=(char *)cupsGetOption("printer-make-and-model",Destinos[i].num_options,Destinos[i].options);
        DetalleImpresora[i].Informacion=(char *)cupsGetOption("printer-info",Destinos[i].num_options,Destinos[i].options);
        DetalleImpresora[i].Localizacion=(char *)cupsGetOption("printer-location",Destinos[i].num_options,Destinos[i].options);
    }
    
    gtk_label_set_text(labTituloImpresora,DetalleImpresora[0].Modelo);
    ptr = strstr(DetalleImpresora[0].Modelo, "E");

    if ( ptr != 0 ) 
    {
        gtk_image_set_from_file(imgImpresora,"glade/Epson.png");
    }
    else
    {
        gtk_image_set_from_file(imgImpresora,"glade/hp.png");
    }
       
    if (inhiboCancelar)
    {
        gtk_widget_hide(GTK_WIDGET(btnCancelar));
    }
    
    gtk_dialog_run(selImpresora);
    return 1;
}

void on_btnCancelar_clicked()
{
    gtk_widget_hide(GTK_WIDGET(selImpresora));
    //cupsFreeDests(cantDestinos,Destinos);  
}

void on_btnAceptar_clicked ()
{
    strcpy(link2,"");
    Impresora = gtk_combo_box_text_get_active_text(cmbImpresora);
    item = gtk_combo_box_get_active(GTK_COMBO_BOX(cmbImpresora));
    strcat(link2,"http://localhost:631/printers/");
    strcat(link2,Impresora);
    gtk_widget_hide(GTK_WIDGET(selImpresora));
    gtk_button_set_label(GTK_BUTTON(linkImpresora),Impresora);
    gtk_link_button_set_uri(linkImpresora,link2);
}

void mostrar_selector()
{
    gtk_widget_show(GTK_WIDGET(selImpresora));
}

void on_cmbImpresora_changed()
{
    int i=0;
    char *ptr;

    i=gtk_combo_box_get_active(GTK_COMBO_BOX(cmbImpresora));
    gtk_label_set_text(labTituloImpresora,DetalleImpresora[i].Modelo);
    ptr = strstr(DetalleImpresora[i].Modelo, "E");

    if ( ptr != 0 ) 
    {
        gtk_image_set_from_file(imgImpresora,"glade/Epson.png");
    }
    else
    {
        gtk_image_set_from_file(imgImpresora,"glade/hp.png");
    }
    
}