import { Component, OnInit } from '@angular/core';
import { FormBuilder, FormGroup, Validators, ReactiveFormsModule } from '@angular/forms';
import { Router } from '@angular/router';
import { AuthServiceService } from 'src/app/services/auth-service.service';

@Component({
  selector: 'app-signup',
  templateUrl: './signup.component.html',
  styleUrls: ['./signup.component.scss']
})
export class SignupComponent implements OnInit {

  registerForm: FormGroup |any;
  error: any;

  constructor(private auth: AuthServiceService,
    private router: Router,
    private formBuilder: FormBuilder) {
    if(this.auth.currentUserValue){
      this.router.navigate(['/data-visual']);
    }
   }

  ngOnInit(): void {
    this.registerForm = this.formBuilder.group({
      username: ['', [Validators.required]],
      password: ['', Validators.required]
    });
  }

  
  goToLogin(){
    this.router.navigate(["/login"]);
  }


  onSubmit(){
    let body = this.registerForm.value; 

    this.auth.signup(body).subscribe((data: any) => {
      
      if(data.id){
        this.router.navigate(["/login"]);
      } 
    }, (err) => {
      alert(err.message);
    })
  }
}
