import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { HttpClientModule } from '@angular/common/http';
import {MatToolbarModule} from '@angular/material/toolbar';
import {MatIconModule} from '@angular/material/icon'
import {MatButtonModule} from '@angular/material/button/'
import {MatPaginatorModule} from '@angular/material/paginator'
import {ChartsModule} from 'ng2-charts';
import {MatTableModule} from '@angular/material/table'
import {MatSlideToggleModule} from '@angular/material/slide-toggle'


import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { DataVisualComponent } from './pages/data-visual/data-visual.component';
import { LayoutComponent } from './layout/layout.component';
import { MenuComponent } from './components/menu/menu.component';
import { NoopAnimationsModule } from '@angular/platform-browser/animations';

@NgModule({
  declarations: [
    AppComponent,
    DataVisualComponent,
    LayoutComponent,
    MenuComponent
  ],
  imports: [
    BrowserModule,
    AppRoutingModule,
    HttpClientModule,
    NoopAnimationsModule,
    MatToolbarModule, 
    MatIconModule,
    MatButtonModule,
    ChartsModule,
    MatPaginatorModule,
    MatTableModule,
    MatSlideToggleModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
