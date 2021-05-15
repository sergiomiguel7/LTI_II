import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { LayoutComponent } from './layout/layout.component';
import {DataVisualComponent} from './pages/data-visual/data-visual.component';

const routes: Routes = [{
  path: '', component: LayoutComponent,
  children: [
    {path: 'data-visual',component: DataVisualComponent}
  ]
}];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
