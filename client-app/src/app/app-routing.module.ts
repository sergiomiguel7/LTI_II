import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { AuthGuard } from './guards/auth.guard';
import { LayoutComponent } from './layout/layout.component';
import { DataVisualComponent } from './pages/data-visual/data-visual.component';
import { LoginComponent } from './pages/login/login.component';

const routes: Routes = [{
  path: '', component: LayoutComponent,
  children: [
    { path: 'login', component: LoginComponent },
    { path: 'data-visual', component: DataVisualComponent, canActivate: [AuthGuard] }
  ]
},
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
