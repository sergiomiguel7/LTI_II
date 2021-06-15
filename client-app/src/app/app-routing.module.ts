import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { AuthGuard } from './guards/auth.guard';
import { LayoutComponent } from './layout/layout.component';
import { AdminPageComponent } from './pages/admin-page/admin-page.component';
import { DataVisualComponent } from './pages/data-visual/data-visual.component';
import { LoginComponent } from './pages/login/login.component';
import { MotionDetectionComponent } from './pages/motion-detection/motion-detection.component';
import { SignupComponent } from './pages/signup/signup.component';

const routes: Routes = [{
  path: '', component: LayoutComponent,
  children: [
    { path: 'login', component: LoginComponent },
    { path: 'registo', component: SignupComponent },
    { path: 'data-visual', component: DataVisualComponent, canActivate: [AuthGuard] },
    { path: 'motion', component: MotionDetectionComponent, canActivate: [AuthGuard] },
    { path: 'admin', component: AdminPageComponent, canActivate: [AuthGuard] },
  ]
},
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
