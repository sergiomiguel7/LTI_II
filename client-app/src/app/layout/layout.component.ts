import { Component, OnInit } from '@angular/core';
import { Router } from '@angular/router';
import { AuthServiceService } from '../services/auth-service.service';

@Component({
  selector: 'app-layout',
  templateUrl: './layout.component.html',
  styleUrls: ['./layout.component.scss']
})
export class LayoutComponent implements OnInit {

  constructor(private auth: AuthServiceService,
    private router: Router) { }

  ngOnInit(): void {
    if(!this.auth.getLoggedInUser()){
      this.router.navigate(["/login"]);
    }
    else 
    this.router.navigate(["/data-visual"]);
  }

}
