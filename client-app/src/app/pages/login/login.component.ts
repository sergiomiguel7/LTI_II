import { Component, OnInit } from '@angular/core';
import { FormBuilder, FormGroup, Validators, ReactiveFormsModule } from '@angular/forms';
import { Router } from '@angular/router';
import { AuthServiceService } from 'src/app/services/auth-service.service';


@Component({
  selector: 'app-login',
  templateUrl: './login.component.html',
  styleUrls: ['./login.component.scss']
})
export class LoginComponent implements OnInit {

  loginForm: FormGroup |any;
  error: any;

  constructor(private auth: AuthServiceService,
    private router: Router,
    private formBuilder: FormBuilder) {
    if(this.auth.currentUserValue){
      this.router.navigate(['/data-visual']);
    }
   }

  ngOnInit(): void {
    this.loginForm = this.formBuilder.group({
      username: ['', [Validators.required]],
      password: ['', Validators.required]
    });
  }

  goToRegister(){
    this.router.navigate(["/registo"]);
  }

  onSubmit(){
    let body = this.loginForm.value; 
    this.auth.login(body).subscribe((data: any) => {
      console.log("data", data);
      if(data.token){
        this.auth.changeUser(data);
        this.router.navigate(["/data-visual"]);
      } 
    }, (err) => {
      alert(err.message);
    })
  }

}
