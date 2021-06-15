import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { BehaviorSubject, Observable } from 'rxjs';
import { User } from '../models/user';
import {apiUrl} from '../../environments/environment';

@Injectable({
  providedIn: 'root'
})
export class AuthServiceService {

  private currentUserSubject: BehaviorSubject<User> | any;
  public currentUser$: Observable<User> | any;

  constructor(private http: HttpClient) {
    this.currentUserSubject = new BehaviorSubject<User>(JSON.parse(localStorage.getItem("currentUser") as any));
    this.currentUser$ = this.currentUserSubject.asObservable();
  }


  public get currentUserValue(): User {
    return this.currentUserSubject.value;
  }

  public getLoggedInUser(): any {
    return this.currentUserSubject.value;
  }

  public isLoggedIn(): boolean{    
    return this.currentUserSubject.value ? true : false;
  }

  changeUser(body: any){
    let user = new User();
    user.id = body.id;
    user.role = body.role;
    user.token = body.token;
    localStorage.setItem("currentUser", JSON.stringify(user));
    this.currentUserSubject.next(user);
  }

  login(body: any){
    const url = `${apiUrl}/auth/login`;
    const $response = this.http.post(url, body);
    return $response;
  }

  signup(body: any){
    const url = `${apiUrl}/auth/signup`;
    const $response = this.http.post(url, body);
    return $response;
  }

  logout(){
    localStorage.removeItem("currentUser");
    this.currentUserSubject.next(null);
  }
}
