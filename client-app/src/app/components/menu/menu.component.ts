import { Component, OnInit } from '@angular/core';
import { Router } from '@angular/router';
import { User } from 'src/app/models/user';
import { AuthServiceService } from 'src/app/services/auth-service.service';

@Component({
  selector: 'app-menu',
  templateUrl: './menu.component.html',
  styleUrls: ['./menu.component.scss']
})
export class MenuComponent implements OnInit {

  user: User;

  constructor(public authService: AuthServiceService,
    private router: Router) {
    this.user = authService.getLoggedInUser();
   }

  ngOnInit(): void {
  }

  isLoggedIn(){
    return this.authService.isLoggedIn();
  }

  isAdmin(){
    return this.user && this.user.role == 'admin' ? true : false;
  }

  logout(){
    this.authService.logout();
    this.router.navigate(["/login"]);
  }


}
